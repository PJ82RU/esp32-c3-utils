#ifndef ESP32_C3_UTILS_QUEUE_H
#define ESP32_C3_UTILS_QUEUE_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <utility>
#include <esp_log.h>
#include <type_traits>

namespace esp32_c3::objects
{
    /// @brief Результат выполнения операции receive
    enum class QueueReceiveResult
    {
        SUCCESS,    ///< Элемент успешно получен
        TIMEOUT,    ///< Истекло время ожидания
        ABORTED,    ///< Операция прервана вызовом abortReceive
        QUEUE_ERROR ///< Ошибка очереди (не валидный handle)
    };

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
        explicit Queue(const UBaseType_t queueLength) noexcept
        {
            static_assert(std::is_trivially_copyable_v<T>,
                          "Queue elements must be trivially copyable");

            mHandle = xQueueCreate(queueLength, sizeof(T));
            if (mHandle == nullptr)
            {
                ESP_LOGE(TAG, "Queue creation failed (length=%u, size=%zu)",
                         queueLength, sizeof(T));
            }
            else
            {
                ESP_LOGD(TAG, "Queue created (length=%u)", queueLength);
            }
        }

        ~Queue() noexcept
        {
            cleanup();
        }

        // Запрет копирования
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;

        // Поддержка перемещения
        Queue(Queue&& other) noexcept :
            mHandle(std::exchange(other.mHandle, nullptr)),
            mAbortFlag(other.mAbortFlag.load())
        {
        }

        Queue& operator=(Queue&& other) noexcept
        {
            if (this != &other)
            {
                cleanup();
                mHandle = std::exchange(other.mHandle, nullptr);
                mAbortFlag = other.mAbortFlag.load();
            }
            return *this;
        }

        /**
         * @brief Проверка валидности очереди
         * @return true если очередь создана успешно
         */
        [[nodiscard]] bool isValid() const noexcept
        {
            return mHandle != nullptr;
        }

        /**
         * @brief Отправить элемент в очередь
         * @param item Элемент для отправки
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        bool send(const T& item, const TickType_t ticksToWait = 0) const noexcept
        {
            return mHandle && xQueueSend(mHandle, &item, ticksToWait) == pdTRUE;
        }

        /**
         * @brief Перезаписать элемент (для очередей длиной 1)
         * @param item Новый элемент
         * @return true если успешно
         */
        bool overwrite(const T& item) const noexcept
        {
            return mHandle && xQueueOverwrite(mHandle, &item) == pdTRUE;
        }

        /**
         * @brief Получить элемент из очереди
         * @param item Ссылка для сохранения элемента
         * @param ticksToWait Время ожидания
         * @return Результат операции receive
         */
        [[nodiscard]] QueueReceiveResult receive(T& item, const TickType_t ticksToWait = portMAX_DELAY) const noexcept
        {
            if (!mHandle) return QueueReceiveResult::QUEUE_ERROR;

            const BaseType_t result = xQueueReceive(mHandle, &item, ticksToWait);

            if (mAbortFlag.load())
            {
                mAbortFlag.store(false);
                xQueueReset(mHandle); // Очищаем очередь после прерывания
                return QueueReceiveResult::ABORTED;
            }

            return result == pdTRUE ? QueueReceiveResult::SUCCESS : QueueReceiveResult::TIMEOUT;
        }

        /**
         * @brief Прервать блокирующую операцию receive и очистить очередь
         * @return true если успешно
         */
        bool reset() const noexcept
        {
            if (!mHandle) return false;

            mAbortFlag.store(true);

            // Отправляем любой элемент чтобы разблокировать ожидание
            T dummy{};
            return xQueueSendToFront(mHandle, &dummy, 0) == pdTRUE;
        }

        /// @brief Количество элементов в очереди
        [[nodiscard]] UBaseType_t messagesWaiting() const noexcept
        {
            return mHandle ? uxQueueMessagesWaiting(mHandle) : 0;
        }

        /// @brief Количество свободных мест
        [[nodiscard]] UBaseType_t spacesAvailable() const noexcept
        {
            return mHandle ? uxQueueSpacesAvailable(mHandle) : 0;
        }

    private:
        void cleanup() noexcept
        {
            if (mHandle)
            {
                vQueueDelete(mHandle);
                mHandle = nullptr;
                ESP_LOGD(TAG, "Queue deleted");
            }
        }

        QueueHandle_t mHandle = nullptr;
        mutable std::atomic<bool> mAbortFlag{false};
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_QUEUE_H