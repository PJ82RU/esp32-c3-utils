#include "esp32_c3_objects/queue.h"
#include <stdexcept>
#include <esp_log.h>

namespace esp32_c3_objects
{
    Queue::Queue(const UBaseType_t queueLength, const UBaseType_t itemSize)
    {
        mHandle = xQueueCreate(queueLength, itemSize);
        if (mHandle == nullptr)
        {
            log_e("Failed to create queue");
            throw std::runtime_error("Failed to create queue");
        }
        log_d("Queue created, length=%u, itemSize=%u", queueLength, itemSize);
    }

    Queue::~Queue() noexcept
    {
        cleanup();
    }

    Queue::Queue(Queue&& other) noexcept
        : mHandle(other.mHandle)
    {
        other.mHandle = nullptr;
    }

    Queue& Queue::operator=(Queue&& other) noexcept
    {
        if (this != &other)
        {
            cleanup();
            mHandle = other.mHandle;
            other.mHandle = nullptr;
        }
        return *this;
    }

    void Queue::cleanup() noexcept
    {
        if (mHandle)
        {
            vQueueDelete(mHandle);
            mHandle = nullptr;
            log_d("Queue deleted");
        }
    }

    UBaseType_t Queue::messagesWaiting() const noexcept
    {
        return mHandle ? uxQueueMessagesWaiting(mHandle) : 0;
    }

    UBaseType_t Queue::spacesAvailable() const noexcept
    {
        return mHandle ? uxQueueSpacesAvailable(mHandle) : 0;
    }

    bool Queue::send(const void* item, const TickType_t ticksToWait) const noexcept
    {
        return mHandle && (xQueueSend(mHandle, item, ticksToWait) == pdTRUE);
    }

    bool Queue::overwrite(const void* item) const noexcept
    {
        return mHandle && (xQueueOverwrite(mHandle, item) == pdTRUE);
    }

    bool Queue::receive(void* buffer, const TickType_t ticksToWait) const noexcept
    {
        return mHandle && (xQueueReceive(mHandle, buffer, ticksToWait) == pdTRUE);
    }

    void Queue::reset() const noexcept
    {
        if (mHandle)
        {
            xQueueReset(mHandle);
            log_d("Queue reset");
        }
    }
} // namespace esp32_c3_utils
