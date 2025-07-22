#include "esp32_c3_objects/queue.h"

#include <stdexcept>
#include <utility>
#include <esp_log.h>

namespace esp32_c3::objects
{
    template <typename T>
    Queue<T>::Queue(const UBaseType_t queueLength) noexcept
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

    template <typename T>
    Queue<T>::~Queue() noexcept
    {
        cleanup();
    }

    template <typename T>
    Queue<T>::Queue(Queue&& other) noexcept
        : mHandle(std::exchange(other.mHandle, nullptr))
    {
    }

    template <typename T>
    Queue<T>& Queue<T>::operator=(Queue&& other) noexcept
    {
        if (this != &other)
        {
            cleanup();
            mHandle = std::exchange(other.mHandle, nullptr);
        }
        return *this;
    }

    template <typename T>
    bool Queue<T>::isValid() const noexcept
    {
        return mHandle != nullptr;
    }

    template <typename T>
    void Queue<T>::cleanup() noexcept
    {
        if (mHandle)
        {
            vQueueDelete(mHandle);
            mHandle = nullptr;
            ESP_LOGD(TAG, "Queue deleted");
        }
    }

    template <typename T>
    bool Queue<T>::send(const T& item, const TickType_t ticksToWait) const noexcept
    {
        return mHandle && xQueueSend(mHandle, &item, ticksToWait) == pdTRUE;
    }

    template <typename T>
    bool Queue<T>::overwrite(const T& item) const noexcept
    {
        return mHandle && xQueueOverwrite(mHandle, &item) == pdTRUE;
    }

    template <typename T>
    bool Queue<T>::receive(T& item, const TickType_t ticksToWait) const noexcept
    {
        return mHandle && xQueueReceive(mHandle, &item, ticksToWait) == pdTRUE;
    }

    template <typename T>
    UBaseType_t Queue<T>::messagesWaiting() const noexcept
    {
        return mHandle ? uxQueueMessagesWaiting(mHandle) : 0;
    }

    template <typename T>
    UBaseType_t Queue<T>::spacesAvailable() const noexcept
    {
        return mHandle ? uxQueueSpacesAvailable(mHandle) : 0;
    }

    template <typename T>
    bool Queue<T>::reset() const noexcept
    {
        if (!mHandle) return false;
        xQueueReset(mHandle);
        return true;
    }
} // namespace esp32_c3::objects
