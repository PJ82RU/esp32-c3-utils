#ifndef ESP32_C3_UTILS_QUEUE_H
#define ESP32_C3_UTILS_QUEUE_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>

namespace esp32_c3::objects
{
    /**
     * @brief Класс-обертка для работы с очередью FreeRTOS
     *
     * @details Предоставляет безопасный интерфейс для работы с очередями FreeRTOS
     *          с поддержкой RAII и семантики перемещения. Гарантирует освобождение
     *          ресурсов при уничтожении объекта.
     */
    class Queue
    {
    public:
        static constexpr auto TAG = "Queue"; ///< Тег для логирования

        /**
         * @brief Конструктор очереди FreeRTOS
         * @param queueLength Максимальное количество элементов в очереди
         * @param itemSize Размер каждого элемента в байтах
         * @throws std::runtime_error Если создание очереди не удалось
         */
        Queue(UBaseType_t queueLength, UBaseType_t itemSize);

        /// @brief Деструктор - автоматически удаляет очередь FreeRTOS
        ~Queue() noexcept;

        // Запрет копирования
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;

        /**
         * @brief Конструктор перемещения
         * @param other Объект для перемещения
         */
        Queue(Queue&& other) noexcept;

        /**
         * @brief Оператор перемещения
         * @param other Объект для перемещения
         * @return Ссылка на текущий объект
         */
        Queue& operator=(Queue&& other) noexcept;

        /**
         * @brief Получить количество элементов в очереди
         * @return Количество элементов, ожидающих обработки
         */
        [[nodiscard]] UBaseType_t messagesWaiting() const noexcept;

        /**
         * @brief Получить количество свободных мест в очереди
         * @return Количество доступных слотов для новых элементов
         */
        [[nodiscard]] UBaseType_t spacesAvailable() const noexcept;

        /**
         * @brief Отправить элемент в очередь
         * @param item Указатель на отправляемые данные
         * @param ticksToWait Время ожидания в тиках (0 - не ждать)
         * @return true если отправка успешна, false при ошибке или таймауте
         */
        [[nodiscard]] bool send(const void* item, TickType_t ticksToWait = 0) const noexcept;

        /**
         * @brief Перезаписать элемент в очереди (для очередей длиной 1)
         * @param item Указатель на новые данные
         * @return true если операция успешна
         * @note Используется только для очередей размером 1
         */
        [[nodiscard]] bool overwrite(const void* item) const noexcept;

        /**
         * @brief Получить элемент из очереди
         * @param buffer Буфер для принятых данных
         * @param ticksToWait Время ожидания в тиках (portMAX_DELAY - ждать вечно)
         * @return true если получение успешно, false при ошибке или таймауте
         */
        [[nodiscard]] bool receive(void* buffer, TickType_t ticksToWait = portMAX_DELAY) const noexcept;

        /**
         * @brief Очистить очередь
         * @note Удаляет все элементы из очереди
         */
        void reset() const noexcept;

    private:
        /**
         * @brief Внутренняя функция для освобождения ресурсов
         * @note Вызывается при уничтожении или перемещении объекта
         */
        void cleanup() noexcept;

        QueueHandle_t mHandle = nullptr; ///< Хэндл очереди FreeRTOS
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_QUEUE_H
