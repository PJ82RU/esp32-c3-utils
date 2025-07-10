#include "esp32_c3_objects/queue.h"

#include <stdexcept>
#include <utility>

namespace esp32_c3::objects
{
    Queue::Queue(const UBaseType_t queueLength, const UBaseType_t itemSize)
    {
        mHandle = xQueueCreate(queueLength, itemSize);
        if (mHandle == nullptr)
        {
            ESP_LOGE(TAG, "Failed to create queue (length=%u, itemSize=%u)", queueLength, itemSize);
        }
        else
        {
            ESP_LOGD(TAG, "Queue created (length=%u, itemSize=%u)", queueLength, itemSize);
        }
    }

    Queue::~Queue() noexcept
    {
        cleanup();
    }

    Queue::Queue(Queue&& other) noexcept
        : mHandle(std::exchange(other.mHandle, nullptr))
    {
    }

    Queue& Queue::operator=(Queue&& other) noexcept
    {
        if (this != &other)
        {
            cleanup();
            mHandle = std::exchange(other.mHandle, nullptr);
        }
        return *this;
    }

    void Queue::cleanup() noexcept
    {
        if (mHandle != nullptr)
        {
            vQueueDelete(mHandle);
            mHandle = nullptr;
            ESP_LOGD(TAG, "Queue deleted");
        }
    }

    UBaseType_t Queue::messagesWaiting() const noexcept
    {
        return mHandle != nullptr ? uxQueueMessagesWaiting(mHandle) : 0;
    }

    UBaseType_t Queue::spacesAvailable() const noexcept
    {
        return mHandle != nullptr ? uxQueueSpacesAvailable(mHandle) : 0;
    }

    bool Queue::send(const void* item, const TickType_t ticksToWait) const noexcept
    {
        return mHandle != nullptr && (xQueueSend(mHandle, item, ticksToWait) == pdTRUE);
    }

    bool Queue::overwrite(const void* item) const noexcept
    {
        return mHandle != nullptr && (xQueueOverwrite(mHandle, item) == pdTRUE);
    }

    bool Queue::receive(void* buffer, const TickType_t ticksToWait) const noexcept
    {
        return mHandle != nullptr && (xQueueReceive(mHandle, buffer, ticksToWait) == pdTRUE);
    }

    void Queue::reset() const noexcept
    {
        if (mHandle != nullptr)
        {
            xQueueReset(mHandle);
            ESP_LOGD(TAG, "Queue reset");
        }
    }
} // namespace esp32_c3::objects
