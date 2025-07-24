#ifndef ESP32_C3_UTILS_BUFFERED_QUEUE_H
#define ESP32_C3_UTILS_BUFFERED_QUEUE_H

#pragma once

#include "queue.h"
#include <memory>
#include "esp_log.h"

namespace esp32_c3::objects
{
    /**
     * @brief Потокобезопасная буферизированная очередь фиксированного размера
     * @tparam T Тип элементов очереди
     * @tparam BufferSize Максимальный размер буфера
     */
    template <typename T, size_t BufferSize>
    class BufferedQueue
    {
    public:
        /// @brief Тег для логирования
        static constexpr auto TAG = "BufferedQueue";

        /**
         * @brief Конструктор
         * @param queueLength Максимальное количество элементов в очереди
         */
        explicit BufferedQueue(UBaseType_t queueLength) noexcept
            : mQueue(queueLength),
              mFreeIndices(BufferSize),
              mBuffer(std::make_unique<T[]>(BufferSize))
        {
            // Проверка успешности создания всех компонентов
            mInitialized = mQueue.isValid() && mFreeIndices.isValid() && mBuffer;

            if (mInitialized)
            {
                // Инициализация свободных индексов
                for (size_t i = 0; i < BufferSize; ++i)
                {
                    if (!mFreeIndices.send(i, 0))
                    {
                        mInitialized = false;
                        break;
                    }
                }
            }

            ESP_LOGD(TAG, "BufferedQueue %s initialized (buffer size: %zu)",
                     mInitialized ? "successfully" : "failed to", BufferSize);
        }

        /**
         * @brief Проверка валидности очереди
         * @return true если все компоненты инициализированы успешно
         */
        [[nodiscard]] bool isValid() const noexcept
        {
            return mInitialized;
        }

        /**
         * @brief Отправить элемент в очередь
         * @param item Элемент для отправки
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        bool send(const T& item, TickType_t ticksToWait = portMAX_DELAY) noexcept
        {
            if (!mInitialized) return false;

            size_t index;
            if (!getFreeIndex(index, ticksToWait))
            {
                ESP_LOGW(TAG, "Failed to get free index");
                return false;
            }

            // Копируем данные в буфер
            mBuffer[index] = item;

            // Отправляем индекс в очередь
            if (QueueItem qi{index}; !mQueue.send(qi, ticksToWait))
            {
                ESP_LOGE(TAG, "Failed to send item to queue");
                returnFreeIndex(index);
                return false;
            }

            return true;
        }

        /**
         * @brief Получить элемент из очереди
         * @param item Ссылка для сохранения элемента
         * @param ticksToWait Время ожидания
         * @return true если успешно
         */
        bool receive(T& item, TickType_t ticksToWait = portMAX_DELAY) noexcept
        {
            if (!mInitialized) return false;

            switch (QueueItem qi; mQueue.receive(qi, ticksToWait))
            {
            case QueueReceiveResult::SUCCESS:
                // Копируем данные из буфера и возвращаем индекс в пул
                item = mBuffer[qi.index];
                returnFreeIndex(qi.index);
                return true;

            case QueueReceiveResult::ABORTED:
                ESP_LOGD(TAG, "Receive operation aborted");
                break;

            case QueueReceiveResult::TIMEOUT:
                ESP_LOGD(TAG, "Receive operation timeout");
                break;

            case QueueReceiveResult::QUEUE_ERROR:
                ESP_LOGE(TAG, "Queue error");
                break;
            default: ;
            }
            return false;
        }

        /**
         * @brief Прервать блокирующую операцию receive и очистить очередь
         * @return true если успешно
         */
        bool reset() const noexcept
        {
            return mInitialized && mQueue.reset();
        }

        /// @brief Количество свободных мест в очереди
        [[nodiscard]] size_t available() const noexcept
        {
            return mInitialized ? mQueue.spacesAvailable() : 0;
        }

        /// @brief Количество ожидающих элементов
        [[nodiscard]] size_t waiting() const noexcept
        {
            return mInitialized ? mQueue.messagesWaiting() : 0;
        }

        /**
         * @brief Проверка, пуста ли очередь
         * @return true если очередь пуста или не инициализирована
         */
        [[nodiscard]] bool empty() const noexcept
        {
            return !mInitialized || (mQueue.messagesWaiting() == 0);
        }

    private:
        struct QueueItem
        {
            size_t index; ///< Индекс элемента в буфере
        };

        bool getFreeIndex(size_t& index, const TickType_t ticksToWait) const noexcept
        {
            return mInitialized && mFreeIndices.receive(index, ticksToWait) == QueueReceiveResult::SUCCESS;
        }

        void returnFreeIndex(const size_t index) const noexcept
        {
            if (mInitialized)
            {
                (void)mFreeIndices.send(index, 0);
            }
        }

        Queue<QueueItem> mQueue;      ///< Основная очередь
        Queue<size_t> mFreeIndices;   ///< Очередь свободных индексов
        std::unique_ptr<T[]> mBuffer; ///< Буфер данных
        bool mInitialized = false;    ///< Флаг успешной инициализации
    };
} // namespace esp32_c3::objects

#endif //ESP32_C3_UTILS_BUFFERED_QUEUE_H
