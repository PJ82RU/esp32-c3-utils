#ifndef ESP32_C3_UTILS_THREAD_H
#define ESP32_C3_UTILS_THREAD_H

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace esp32_c3::objects
{
    /**
     * @brief Максимальная длина имени потока (включая нуль-терминатор)
     */
    constexpr size_t THREAD_NAME_SIZE = 32;

    /**
     * @brief Класс-обертка для работы с задачами FreeRTOS
     * @details Предоставляет удобный интерфейс для создания и управления задачами,
     * включая контроль стека, приоритетов и привязку к ядрам процессора.
     * Поддерживает ESP-IDF v5+ и использует возможности C++17.
     */
    class Thread
    {
    public:
        /// @brief Действия для цикла выполнения задачи
        enum class LoopAction
        {
            CONTINUE, ///< Продолжить выполнение
            PAUSE,    ///< Приостановить выполнение
            STOP      ///< Остановить поток
        };

        /// @brief Состояния потока
        enum class State
        {
            NOT_RUNNING, ///< Поток не запущен
            READY,       ///< Поток готов к выполнению
            RUNNING,     ///< Поток выполняется
            SUSPENDED    ///< Поток приостановлен
        };

        /// @brief Тег для логирования
        static constexpr auto TAG = "Thread";

        /// @brief Тип функции цикла выполнения
        using LoopFunc = std::function<LoopAction()>;

        /**
         * @brief Конструктор задачи FreeRTOS
         * @param name Имя задачи (максимум 31 символ + нуль-терминатор)
         * @param stackDepth Размер стека в словах (4 байта на слово)
         * @param priority Приоритет задачи (0 - самый низкий)
         */
        explicit Thread(std::string_view name, uint32_t stackDepth, UBaseType_t priority) noexcept;

        /// @brief Деструктор - автоматически останавливает задачу
        ~Thread() noexcept;

        // Запрещаем копирование и перемещение
        Thread(const Thread&) = delete;
        Thread(Thread&&) = delete;
        Thread& operator=(const Thread&) = delete;
        Thread& operator=(Thread&&) = delete;

        /**
         * @brief Запуск задачи с циклом выполнения
         * @param loopFunc Функция цикла выполнения
         * @param intervalMs Интервал выполнения цикла в миллисекундах
         * @param startPaused Запустить в приостановленном состоянии
         * @return Код ошибки ESP_OK в случае успеха
         */
        [[nodiscard]] esp_err_t start(LoopFunc loopFunc, uint32_t intervalMs = 10, bool startPaused = false) noexcept;

        /**
         * @brief Запуск задачи на любом доступном ядре
         * @param taskFunc Функция-задача (бесконечный цикл)
         * @param params Параметры для передачи в задачу
         * @return Код ошибки ESP_OK в случае успеха
         */
        [[nodiscard]] esp_err_t start(TaskFunction_t taskFunc, void* params) noexcept;

        /**
         * @brief Запуск задачи с привязкой к конкретному ядру
         * @param taskFunc Функция-задача (бесконечный цикл)
         * @param params Параметры для передачи в задачу
         * @param coreId Номер ядра (0 или 1)
         * @return Код ошибки ESP_OK в случае успеха
         */
        [[nodiscard]] esp_err_t start(TaskFunction_t taskFunc, void* params, BaseType_t coreId) noexcept;

        /**
         * @brief Остановка и удаление задачи
         * @param softStop Флаг мягкой остановки (true - ожидание завершения, false - принудительная)
         * @note Безопасно вызывать даже если задача не запущена
         */
        void stop(bool softStop = true) noexcept;

        /**
         * @brief Получение текущего состояния потока
         * @return Состояние потока
         */
        [[nodiscard]] State state() const noexcept;

        /**
         * @brief Приостановка выполнения задачи
         * @note Задачу можно возобновить методом resume()
         */
        void suspend() const noexcept;

        /**
         * @brief Возобновление приостановленной задачи
         */
        void resume() const noexcept;

        /**
         * @brief Изменить приоритет задачи
         * @param newPriority Новый приоритет
         * @return Код ошибки ESP_OK в случае успеха
         */
        [[nodiscard]] esp_err_t setPriority(UBaseType_t newPriority) noexcept;

        /**
         * @brief Получение размера стека задачи
         * @return Размер стека в словах (4 байта на слово)
         */
        [[nodiscard]] uint32_t stackSize() const noexcept;

        /**
         * @brief Получение минимального свободного места в стеке
         * @return Минимальное количество оставшихся слов стека
         * @note Полезно для анализа использования стека
         */
        [[nodiscard]] UBaseType_t stackHighWaterMark() const noexcept;

        /**
         * @brief Получение имени задачи
         * @return Указатель на имя задачи
         */
        [[nodiscard]] const char* name() const noexcept;

    private:
        /**
         * @brief Контекст для цикла выполнения
         */
        struct LoopContext
        {
            LoopFunc func;                   ///< Функция цикла
            TickType_t interval;             ///< Интервал выполнения
            Thread* thread;                  ///< Указатель на родительский объект
            std::atomic<bool> shouldStop;    ///< Флаг остановки
            std::atomic<bool> isStartPaused; ///< Флаг старта в приостановленном состоянии

            // Явно объявляем конструктор
            LoopContext(LoopFunc&& f, const TickType_t i, Thread* t, const bool stop, const bool paused) :
                func(std::move(f)),
                interval(i),
                thread(t),
                shouldStop(stop),
                isStartPaused(paused)
            {
            }
        };

        /**
         * @brief Обертка для функции цикла выполнения
         * @param arg Указатель на контекст LoopContext
         */
        static void loopWrapper(void* arg) noexcept;

        // Примитивные типы
        uint32_t mStackDepth;  ///< Запрошенный размер стека
        UBaseType_t mPriority; ///< Приоритет задачи

        // Контейнеры
        std::array<char, THREAD_NAME_SIZE> mName; ///< Имя задачи (для отладки)

        // Указатели
        std::atomic<TaskHandle_t> mHandle{nullptr}; ///< Хэндл задачи FreeRTOS
        std::unique_ptr<LoopContext> mLoopContext;  ///< Контекст цикла выполнения
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_THREAD_H
