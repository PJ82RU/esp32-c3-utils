#include "esp32_c3_objects/buffered_queue.h"
#include "esp_log.h"

namespace esp32_c3::objects
{
    template <typename T, size_t BufferSize>
    BufferedQueue<T, BufferSize>::BufferedQueue(UBaseType_t queueLength) noexcept
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

    template <typename T, size_t BufferSize>
    bool BufferedQueue<T, BufferSize>::isValid() const noexcept
    {
        return mInitialized;
    }

    template <typename T, size_t BufferSize>
    bool BufferedQueue<T, BufferSize>::send(const T& item, TickType_t ticksToWait) noexcept
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

    template <typename T, size_t BufferSize>
    bool BufferedQueue<T, BufferSize>::receive(T& item, TickType_t ticksToWait) noexcept
    {
        if (!mInitialized) return false;

        QueueItem qi;
        if (!mQueue.receive(qi, ticksToWait))
        {
            ESP_LOGW(TAG, "Failed to receive from queue");
            return false;
        }

        // Копируем данные из буфера
        item = mBuffer[qi.index];

        // Возвращаем индекс в пул
        returnFreeIndex(qi.index);
        return true;
    }

    template <typename T, size_t BufferSize>
    size_t BufferedQueue<T, BufferSize>::available() const noexcept
    {
        return mInitialized ? mQueue.spacesAvailable() : 0;
    }

    template <typename T, size_t BufferSize>
    size_t BufferedQueue<T, BufferSize>::waiting() const noexcept
    {
        return mInitialized ? mQueue.messagesWaiting() : 0;
    }

    template <typename T, size_t BufferSize>
    bool BufferedQueue<T, BufferSize>::getFreeIndex(size_t& index, const TickType_t ticksToWait) const noexcept
    {
        return mInitialized && mFreeIndices.receive(index, ticksToWait);
    }

    template <typename T, size_t BufferSize>
    void BufferedQueue<T, BufferSize>::returnFreeIndex(const size_t index) const noexcept
    {
        if (mInitialized)
        {
            (void)mFreeIndices.send(index, 0);
        }
    }
} // namespace esp32_c3::objects
