#ifndef ESP32_C3_UTILS_QUEUE_H
#define ESP32_C3_UTILS_QUEUE_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace esp32_c3::objects
{
    /**
     * @brief Типобезопасная обертка для очереди FreeRTOS
     * @tparam T Тип элементов очереди (должен быть тривиально копируемым)
     */
    template <typename T>
    class Queue
    {
    public:
        /// @brief Тег для логирования
        static constexpr auto TAG = "Queue";

        /**
         * @brief Конструктор очереди
         * @param queueLength Максимальное количество элементов
         */
        explicit Queue(UBaseType_t queueLength) noexcept;

        ~Queue() noexcept;

        // Запрет копирования
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;

        // Поддержка перемещения
        Queue(Queue&& other) noexcept;
        Queue& operator=(Queue&& other) noexcept;

        /**
         * @brief Проверка валидности очереди
         * @return true если очередь создана успешно
         */
        [[nodiscard]] bool isValid() const noexcept;

        /**
         * @brief Отправить элемент в очередь
         * @param item Элемент для отправки
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        bool send(const T& item, TickType_t ticksToWait = 0) const noexcept;

        /**
         * @brief Перезаписать элемент (для очередей длиной 1)
         * @param item Новый элемент
         * @return true если успешно
         */
        bool overwrite(const T& item) const noexcept;

        /**
         * @brief Получить элемент из очереди
         * @param item Ссылка для сохранения элемента
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        [[nodiscard]] bool receive(T& item, TickType_t ticksToWait = portMAX_DELAY) const noexcept;

        /// @brief Количество элементов в очереди
        [[nodiscard]] UBaseType_t messagesWaiting() const noexcept;

        /// @brief Количество свободных мест
        [[nodiscard]] UBaseType_t spacesAvailable() const noexcept;

        /// @brief Очистить очередь
        bool reset() const noexcept;

    private:
        void cleanup() noexcept;
        QueueHandle_t mHandle = nullptr;
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_QUEUE_H
