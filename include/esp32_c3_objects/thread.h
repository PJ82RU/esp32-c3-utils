#ifndef ESP32_C3_UTILS_THREAD_H
#define ESP32_C3_UTILS_THREAD_H

#include <Arduino.h>

namespace esp32_c3_objects
{
    /**
     * @brief Максимальная длина имени потока (включая нуль-терминатор)
     */
    constexpr size_t THREAD_NAME_SIZE = 32;

    /**
     * @brief Класс-обертка для работы с задачами FreeRTOS
     *
     * @details Предоставляет удобный интерфейс для создания и управления задачами,
     * включая контроль стека, приоритетов и привязку к ядрам процессора
     */
    class Thread
    {
    public:
        /**
         * @brief Конструктор задачи FreeRTOS
         * @param name Имя задачи (максимум 31 символ + нуль-терминатор)
         * @param stackDepth Размер стека в словах (4 байта на слово)
         * @param priority Приоритет задачи (0 - самый низкий)
         */
        Thread(const char* name, uint32_t stackDepth, UBaseType_t priority) noexcept;

        /// @brief Деструктор - автоматически останавливает задачу
        ~Thread() noexcept;

        // Запрещаем копирование
        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        /**
         * @brief Запуск задачи на любом доступном ядре
         * @param taskFunc Функция-задача (бесконечный цикл)
         * @param params Параметры для передачи в задачу
         * @return true если задача успешно создана
         */
        [[nodiscard]] bool start(TaskFunction_t taskFunc, void* params) noexcept;

        /**
         * @brief Запуск задачи с привязкой к конкретному ядру
         * @param taskFunc Функция-задача (бесконечный цикл)
         * @param params Параметры для передачи в задачу
         * @param coreId Номер ядра (0 или 1)
         * @return true если задача успешно создана
         */
        [[nodiscard]] bool start(TaskFunction_t taskFunc, void* params, BaseType_t coreId) noexcept;

        /**
         * @brief Остановка и удаление задачи
         * @note Безопасно вызывать даже если задача не запущена
         */
        void stop() noexcept;

        /**
         * @brief Проверка активности задачи
         * @return true если задача существует и не завершена
         */
        [[nodiscard]] bool isRunning() const noexcept;

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

    private:
        /// Хэндл задачи FreeRTOS
        TaskHandle_t mHandle = nullptr;

        /// Имя задачи (для отладки)
        char mName[THREAD_NAME_SIZE] = {};

        /// Запрошенный размер стека
        uint32_t mStackDepth = 0;

        /// Приоритет задачи
        UBaseType_t mPriority = 0;
    };
} // namespace esp32_c3_utils

#endif // ESP32_C3_UTILS_THREAD_H
