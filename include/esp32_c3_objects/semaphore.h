#ifndef ESP32_C3_UTILS_SEMAPHORE_H
#define ESP32_C3_UTILS_SEMAPHORE_H

#include <Arduino.h>

namespace esp32_c3_objects
{
    /**
     * @brief Класс-обертка для работы с семафорами FreeRTOS
     *
     * @details Предоставляет потокобезопасный интерфейс для работы с семафорами,
     *          включая поддержку рекурсивных мьютексов и отладочной информации
     */
    class Semaphore
    {
    public:
        /**
         * @brief Конструктор семафора/мьютекса
         * @param recursive Флаг создания рекурсивного мьютекса
         * @throws std::runtime_error При ошибке создания семафора
         */
        explicit Semaphore(bool recursive);

        /**
         * @brief Деструктор - автоматически освобождает ресурсы семафора
         */
        ~Semaphore() noexcept;

        // Запрещаем копирование
        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;

        // Разрешаем перемещение
        Semaphore(Semaphore&& other) noexcept;
        Semaphore& operator=(Semaphore&& other) noexcept;

        /**
         * @brief Захват семафора (блокирующий)
         * @param blockTime Время ожидания в тиках (portMAX_DELAY - бесконечно)
         * @return true - успешный захват, false - таймаут или ошибка
         */
        [[nodiscard]] bool take(TickType_t blockTime = portMAX_DELAY) const noexcept;

        /**
         * @brief Захват семафора с отладочной информацией
         * @param blockTime Время ожидания в тиках
         * @param func Имя вызывающей функции (для отладки)
         * @param line Номер строки вызова (для отладки)
         * @return true - успешный захват, false - таймаут или ошибка
         */
        [[nodiscard]] bool take(TickType_t blockTime, const char* func, int line) const noexcept;

        /**
         * @brief Освобождение семафора
         * @return true - успешное освобождение, false - ошибка
         */
        [[nodiscard]] bool give() const noexcept;

        /**
         * @brief Освобождение семафора с отладочной информацией
         * @param func Имя вызывающей функции (для отладки)
         * @param line Номер строки вызова (для отладки)
         * @return true - успешное освобождение, false - ошибка
         */
        [[nodiscard]] bool give(const char* func, int line) const noexcept;

        /**
         * @brief Получение текущего значения счетчика семафора
         * @return Текущее значение счетчика
         */
        [[nodiscard]] unsigned int getCount() const noexcept;

    private:
        /// @brief Внутренняя функция освобождения ресурсов
        void cleanup() const noexcept;

        /// Хэндл семафора FreeRTOS
        SemaphoreHandle_t mHandle = nullptr;

        /// Флаг рекурсивного мьютекса
        bool mIsRecursive = false;
    };
} // namespace esp32_c3_utils

// Макросы для удобного использования семафора с отладочной информацией
#define SEMAPHORE_TAKE(sem) (sem).take(pdMS_TO_TICKS(3000), __PRETTY_FUNCTION__, __LINE__)
#define SEMAPHORE_GIVE(sem) (sem).give(__PRETTY_FUNCTION__, __LINE__)

#endif // ESP32_C3_UTILS_SEMAPHORE_H
