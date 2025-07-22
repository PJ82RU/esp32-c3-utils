#ifndef ESP32_C3_UTILS_CALLBACK_H
#define ESP32_C3_UTILS_CALLBACK_H

#include "thread.h"
#include "buffered_queue.h"
#include "simple_callback.h"

#include <mutex>
#include <cstring>
#include <memory>


namespace esp32_c3::objects
{
    /**
     * @brief Типизированный менеджер callback-функций
     * @tparam T Тип передаваемых данных
     * @tparam P Тип параметров callback-функций (по умолчанию void*)
     */
    template <typename T, typename P = void*>
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
         * @brief Тип callback-функции
         * @param input Входные данные (read-only)
         * @param output Буфер для результата (может быть nullptr)
         * @param params Пользовательские параметры
         * @return true если данные обработаны успешно
         */
        using CallbackFunction = bool (*)(const T* input, T* output, P params) noexcept;

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
                          uint32_t stackDepth = DEFAULT_STACK_DEPTH,
                          UBaseType_t priority = DEFAULT_PRIORITY) noexcept;

        /// @brief Деструктор (освобождает ресурсы)
        ~Callback();

        // Запрещаем копирование объектов
        Callback(const Callback&) = delete;
        Callback& operator=(const Callback&) = delete;

        /**
         * @brief Проверка состояния инициализации
         * @return true если объект инициализирован
         */
        [[nodiscard]] bool isInitialized() const noexcept;

        /**
         * @brief Добавление новой callback-функции
         * @param func Указатель на callback-функцию
         * @param params Параметры для функции (опционально)
         * @param onlyIndex Флаг вызова только по индексу (опционально)
         * @return Индекс добавленной функции или -1 при ошибке
         */
        [[nodiscard]] int16_t addCallback(CallbackFunction func,
                                          P params = P{},
                                          bool onlyIndex = false) const noexcept;

        /**
         * @brief Очистка всех зарегистрированных callback-функций
         */
        void free() const noexcept;

        /**
         * @brief Вызывает callback-цепочку с обработкой данных
         * @param input Входные данные (не изменяются)
         * @param response Колбэк для отправки результата
         * @param index Индекс callback (-1 для всех)
         */
        void invoke(const T* input,
                    SimpleCallback<T>* response = nullptr,
                    int16_t index = -1) noexcept;

        /**
         * @brief Чтение данных из буфера
         * @param value Указатель на буфер для данных
         * @return true если данные успешно прочитаны
         */
        [[nodiscard]] bool read(T* value) const noexcept;

    protected:
        /**
         * @brief Структура элемента callback
         */
        struct Item
        {
            bool onlyIndex;        ///< Флаг вызова только по индексу
            CallbackFunction func; ///< Указатель на callback-функцию
            P params;              ///< Параметры для callback-функции
        };

        /**
         * @brief Структура элемента задачи
         */
        struct TaskItem {
            int16_t itemIndex;
            T data;
            SimpleCallback<T>* response;
        };

        /**
         * @brief Задача для обработки callback-функций
         * @param arg Указатель на объект Callback
         */
        static void callbackTask(void* arg) noexcept;

        /**
         * @brief Обработка элементов callback
         * @param item Элемент задачи для обработки
         */
        void process(const TaskItem& item) const noexcept;

        /**
         * @brief Основной цикл обработки
         */
        void run() const noexcept;

        /// Поток для обработки callback
        Thread mThread;

        /// Очередь для хранения заданий
        BufferedQueue<TaskItem, DEFAULT_BUFFER_SIZE> mQueue;

        /// Мьютекс для синхронизации
        mutable std::recursive_mutex mMutex;

        /// Массив callback-функций
        std::unique_ptr<Item[]> mItems = nullptr;

        /// Количество зарегистрированных callback-функций
        uint8_t mNumItems = 0;

        /// Индекс для быстрого поиска свободного слота
        mutable int16_t mLastFreeIndex = 0;
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_CALLBACK_H
