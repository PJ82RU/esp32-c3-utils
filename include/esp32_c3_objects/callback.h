#ifndef ESP32_C3_UTILS_CALLBACK_H
#define ESP32_C3_UTILS_CALLBACK_H

#include "thread.h"
#include "buffered_queue.h"

#include <mutex>
#include <memory>
#include <vector>
#include <functional>
#include <esp_log.h>

namespace esp32_c3::objects
{
    /**
     * @brief Типизированный менеджер callback-функций
     * @tparam T Тип передаваемых данных
     */
    template <typename T>
    class Callback
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Type T must be trivially copyable");

    public:
        /// @brief Количество callback-функций по умолчанию
        static constexpr uint8_t DEFAULT_CALLBACKS = 10;

        /// @brief Размер буфера данных по умолчанию
        static constexpr uint8_t DEFAULT_BUFFER_SIZE = 5;

        /// @brief Размер стека задачи по умолчанию (в байтах)
        static constexpr uint32_t DEFAULT_STACK_DEPTH = 3072;

        /// @brief Приоритет задачи по умолчанию
        static constexpr UBaseType_t DEFAULT_PRIORITY = 18;

        /// @brief Тег для логирования
        static constexpr auto TAG = "Callback";

        /**
         * @brief Тип callback-функции (лямбда)
         * @param input Входные данные (read-only)
         * @param output Буфер для результата (может быть nullptr)
         * @return true если нужно вернуть output
         */
        using CallbackFunction = std::function<bool(const T& input, T& output)>;

        /**
         * @brief Тип response-функции для возврата результата
         * @param result Результат обработки данных
         */
        using ResponseFunction = std::function<void(const T& result)>;

        /**
         * @brief Конструктор менеджера callback-функций
         * @param bufferSize Количество элементов в буфере (рекомендуется 3-10)
         * @param numCallbacks Максимальное количество callback-функций (рекомендуется 5-15)
         * @param name Имя задачи для отладки (должно быть статической строкой)
         * @param stackDepth Размер стека задачи в байтах (по умолчанию 3072)
         * @param priority Приоритет задачи FreeRTOS (по умолчанию 18)
         *
         * @note Особенности работы:
         * - Выделяет память под буферы сразу в конструкторе
         * - Автоматически запускает worker-поток
         * - При ошибках выделения памяти объект остаётся неработоспособным (isInitialized() = false)
         */
        explicit Callback(const char* name,
                          uint8_t bufferSize = DEFAULT_BUFFER_SIZE,
                          uint8_t numCallbacks = DEFAULT_CALLBACKS,
                          const uint32_t stackDepth = DEFAULT_STACK_DEPTH,
                          const UBaseType_t priority = DEFAULT_PRIORITY) noexcept :
            mThread(name, stackDepth, priority),
            mQueue(bufferSize),
            mItems(numCallbacks > 0 ? std::make_unique<Item[]>(numCallbacks) : nullptr),
            mNumItems(numCallbacks)
        {
            if (mQueue.isValid() && numCallbacks > 0)
            {
                ESP_LOGI(TAG, "Constructed with buffer size %d and %d callbacks",
                         bufferSize, numCallbacks);
                free();
            }
            else
            {
                ESP_LOGE(TAG, "Memory allocation failed");
            }
        }

        /// @brief Деструктор (освобождает ресурсы)
        ~Callback()
        {
            stopThread();
            ESP_LOGI(TAG, "Callback destroyed");
        }

        // Запрещаем копирование объектов
        Callback(const Callback&) = delete;
        Callback& operator=(const Callback&) = delete;

        /**
         * @brief Проверка состояния инициализации
         * @return true если объект инициализирован
         */
        [[nodiscard]] bool isInitialized() const noexcept
        {
            std::lock_guard lock(mMutex);
            return mQueue.isValid() && mItems;
        }

        /**
         * @brief Добавление новой callback-функции
         * @param func Лямбда-функция для обработки данных
         * @param onlyIndex Флаг вызова только по индексу (опционально)
         * @return Индекс добавленной функции или -1 при ошибке
         */
        int16_t addCallback(CallbackFunction func,
                            bool onlyIndex = false) noexcept
        {
            if (!isInitialized() || !func || !run()) return -1;

            std::lock_guard lock(mMutex);

            // Быстрый поиск начиная с последнего свободного индекса
            for (int16_t i = 0; i < mNumItems; ++i)
            {
                if (const int16_t currentIndex = (mLastFreeIndex + i) % mNumItems; !mItems[currentIndex].func)
                {
                    mItems[currentIndex] = {onlyIndex, std::move(func)};
                    mLastFreeIndex = (currentIndex + 1) % mNumItems;

                    ESP_LOGD(TAG, "Added callback at index %d", currentIndex);
                    return currentIndex;
                }
            }

            ESP_LOGW(TAG, "No free slots for callback");
            return -1;
        }

        /**
         * @brief Очистка всех зарегистрированных callback-функций
         */
        void free() noexcept
        {
            if (isInitialized())
            {
                stopThread();

                std::lock_guard lock(mMutex);
                if (mItems)
                {
                    for (uint8_t i = 0; i < mNumItems; ++i)
                    {
                        mItems[i] = {};
                    }
                }
                mLastFreeIndex = 0;
                ESP_LOGD(TAG, "Cleared all callbacks");
            }
        }

        /**
         * @brief Вызывает callback-цепочку с обработкой данных
         * @param input Входные данные (не изменяются)
         * @param response Лямбда-функция для обработки результата (опционально)
         * @param index Индекс callback (-1 для всех)
         */
        void invoke(const T& input,
                    ResponseFunction response = nullptr,
                    const int16_t index = -1) noexcept
        {
            if (!isInitialized())
            {
                ESP_LOGE(TAG, "Invoke failed: not initialized or null input");
                return;
            }

            if (TaskItem item{index, std::move(input), std::move(response)}; !mQueue.send(item))
            {
                ESP_LOGE(TAG, "Failed to send item to queue");
            }
        }

        /**
         * @brief Чтение данных из буфера
         * @param value Указатель на буфер для данных
         * @return true если данные успешно прочитаны
         */
        [[nodiscard]] bool read(T& value) const noexcept
        {
            if (isInitialized())
            {
                if (TaskItem item; const_cast<BufferedQueue<TaskItem, DEFAULT_BUFFER_SIZE>&>(mQueue).receive(item))
                {
                    value = item.data;
                    return true;
                }
            }
            return false;
        }

    protected:
        /**
         * @brief Структура элемента callback
         */
        struct Item
        {
            bool onlyIndex;        ///< Флаг вызова только по индексу
            CallbackFunction func; ///< Лямбда-функция
        };

        /**
         * @brief Структура элемента задачи
         */
        struct TaskItem
        {
            int16_t itemIndex;         ///< Индекс callback (-1 для всех)
            T data;                    ///< Передаваемые данные
            ResponseFunction response; ///< Функция для возврата результата
        };

        /**
         * @brief Остановка потока с гарантированным выходом
         */
        void stopThread() noexcept
        {
            mQueue.reset();
            mThread.stop();
        }

        /**
         * @brief Обработка элементов callback
         * @param item Элемент задачи для обработки
         */
        void process(const TaskItem& item) const noexcept
        {
            // 1. Быстро копируем нужные callback'и под блокировкой
            std::vector<Item> activeCallbacks;
            {
                std::lock_guard lock(mMutex);
                activeCallbacks.reserve(mNumItems);
                for (int i = 0; i < mNumItems; ++i)
                {
                    if (const auto& cb = mItems[i]; cb.func && (!cb.onlyIndex || i == item.itemIndex))
                    {
                        activeCallbacks.push_back(cb);
                    }
                }
            }

            // 2. Обрабатываем без блокировки
            for (const auto& cb : activeCallbacks)
            {
                if (T output; cb.func(item.data, output))
                {
                    if (item.response) item.response(output);
                }
            }
        }

        /**
         * @brief Запуск/проверка рабочего потока
         * @return true если поток успешно запущен или уже работает
         */
        bool run()
        {
            if (mThread.state() != Thread::State::NOT_RUNNING) return true;

            auto loop = [&]()
            {
                if (TaskItem item; mQueue.receive(item))
                {
                    process(item);
                }
                return Thread::LoopAction::CONTINUE;
            };

            if (mThread.start(loop) != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to start callback thread");
                return false;
            }
            return true;
        }

        /// Поток для обработки callback
        Thread mThread;

        /// Очередь для хранения заданий
        BufferedQueue<TaskItem, DEFAULT_BUFFER_SIZE> mQueue;

        /// Мьютекс для синхронизации
        mutable std::mutex mMutex;

        /// Массив callback-функций
        std::unique_ptr<Item[]> mItems = nullptr;

        /// Количество зарегистрированных callback-функций
        uint8_t mNumItems = 0;

        /// Индекс для быстрого поиска свободного слота
        mutable int16_t mLastFreeIndex = 0;
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_CALLBACK_H
