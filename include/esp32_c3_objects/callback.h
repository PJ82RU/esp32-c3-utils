#ifndef ESP32_C3_UTILS_CALLBACK_H
#define ESP32_C3_UTILS_CALLBACK_H

#include "thread.h"
#include "queue.h"
#include "simple_callback.h"
#include <mutex>
#include <cstring>

namespace esp32_c3::objects
{
    /**
     * @brief Класс для управления callback-функциями с буферизацией данных
     */
    class Callback
    {
    public:
        /// @brief Размер стека задачи по умолчанию (в байтах)
        static constexpr uint32_t DEFAULT_STACK_DEPTH = 3072;

        /// @brief Приоритет задачи по умолчанию
        static constexpr UBaseType_t DEFAULT_PRIORITY = 18;

        /// @brief Тег для логирования
        static constexpr auto TAG = "Callback";

        /**
         * @brief Тип функции callback
         * @param arg1 Указатель на данные
         * @param arg2 Указатель на параметры
         * @return Результат выполнения (true/false)
         */
        using EventSendFunc = bool (*)(void* arg1, void* arg2);

        /**
         * @brief Структура элемента callback
         */
        struct Item
        {
            bool onlyIndex;     ///< Флаг вызова только по индексу
            EventSendFunc func; ///< Указатель на callback-функцию
            void* params;       ///< Параметры для callback-функции
        };

        /**
         * @brief Структура элемента буфера
         */
        struct BufferItem
        {
            int16_t itemIndex;   ///< Индекс callback-функции
            uint8_t bufferIndex; ///< Индекс в буфере данных
        };

        /**
         * @brief Конструктор менеджера callback-функций
         * @param bufferSize Количество элементов в буфере
         * @param itemSize Размер одного элемента буфера (в байтах)
         * @param name Имя задачи для отладки
         * @param stackDepth Размер стека задачи (по умолчанию 3072)
         * @param priority Приоритет задачи (по умолчанию 18)
         */
        explicit [[nodiscard]] Callback(uint8_t bufferSize,
                                        size_t itemSize,
                                        const char* name,
                                        uint32_t stackDepth = DEFAULT_STACK_DEPTH,
                                        UBaseType_t priority = DEFAULT_PRIORITY) noexcept;

        /// @brief Деструктор (освобождает ресурсы)
        ~Callback();

        // Запрещаем копирование объектов
        Callback(const Callback&) = delete;
        Callback& operator=(const Callback&) = delete;

        /**
         * @brief Инициализация менеджера callback-функций
         * @param numCallbacks Количество callback-функций
         * @return true если инициализация прошла успешно
         */
        [[nodiscard]] bool init(uint8_t numCallbacks) noexcept;

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
        [[nodiscard]] int16_t addCallback(EventSendFunc func,
                                          void* params = nullptr,
                                          bool onlyIndex = false) const noexcept;

        /**
         * @brief Очистка всех зарегистрированных callback-функций
         */
        void free() const noexcept;

        /**
         * @brief Вызов callback-функции/функций
         * @param value Указатель на данные для передачи
         * @param index Индекс callback (-1 для вызова всех)
         */
        void invoke(const void* value, int16_t index = -1) noexcept;

        /**
         * @brief Чтение данных из буфера
         * @param value Указатель на буфер для данных
         * @return true если данные успешно прочитаны
         */
        [[nodiscard]] bool read(void* value) const noexcept;

        /// Родительский callback для обработки событий
        SimpleCallback parentCallback;

    private:
        /**
         * @brief Задача для обработки callback-функций
         * @param arg Указатель на объект Callback
         */
        static void callbackTask(void* arg) noexcept;

        /**
         * @brief Обработка элементов callback
         * @param item Элемент буфера для обработки
         */
        void processItems(const BufferItem& item) const noexcept;

        /**
         * @brief Основной цикл обработки
         */
        void run() const noexcept;

        /// Поток для обработки callback
        Thread mThread;

        /// Очередь для хранения заданий
        Queue mQueue;

        /// Мьютекс для синхронизации
        mutable std::mutex mMutex;

        /// Количество зарегистрированных callback-функций
        uint8_t mNumItems = 0;

        /// Массив callback-функций
        Item* mItems = nullptr;

        /// Размер буфера данных
        uint8_t mBufferSize = 0;

        /// Размер элемента данных
        size_t mItemSize = 0;

        /// Текущий индекс в буфере
        uint8_t mCurrentBufferIndex = 0;

        /// Буфер для хранения данных
        uint8_t* mBuffer = nullptr;
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_CALLBACK_H
