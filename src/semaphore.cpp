#include "esp32_c3_objects/semaphore.h"
#include <esp_log.h>

namespace esp32_c3_objects
{
    Semaphore::Semaphore(const bool recursive) : mIsRecursive(recursive)
    {
        mHandle = mIsRecursive ? xSemaphoreCreateRecursiveMutex() : xSemaphoreCreateMutex();
        if (mHandle == nullptr)
        {
            log_e("Failed to create %s semaphore", mIsRecursive ? "recursive" : "normal");
            throw std::runtime_error("Failed to create semaphore");
        }
        log_d("%s semaphore created", mIsRecursive ? "Recursive" : "Normal");
    }

    Semaphore::~Semaphore() noexcept
    {
        cleanup();
    }

    Semaphore::Semaphore(Semaphore&& other) noexcept
        : mHandle(other.mHandle),
          mIsRecursive(other.mIsRecursive)
    {
        other.mHandle = nullptr;
    }

    Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
    {
        if (this != &other)
        {
            cleanup();
            mHandle = other.mHandle;
            mIsRecursive = other.mIsRecursive;
            other.mHandle = nullptr;
        }
        return *this;
    }

    void Semaphore::cleanup() const noexcept
    {
        if (mHandle)
        {
            vSemaphoreDelete(mHandle);
            log_d("Semaphore deleted");
        }
    }

    bool Semaphore::take(const TickType_t blockTime) const noexcept
    {
        if (!mHandle) return false;

        return mIsRecursive
                   ? xSemaphoreTakeRecursive(mHandle, blockTime) == pdTRUE
                   : xSemaphoreTake(mHandle, blockTime) == pdTRUE;
    }

    bool Semaphore::take(const TickType_t blockTime, const char* func, int line) const noexcept
    {
        const bool result = take(blockTime);
        if (!result && func)
        {
            log_w("[%s:%d] Semaphore take timeout", func, line);
        }
        return result;
    }

    bool Semaphore::give() const noexcept
    {
        if (!mHandle) return false;

        return mIsRecursive
                   ? xSemaphoreGiveRecursive(mHandle) == pdTRUE
                   : xSemaphoreGive(mHandle) == pdTRUE;
    }

    bool Semaphore::give(const char* func, int line) const noexcept
    {
        const bool result = give();
        if (!result && func)
        {
            log_w("[%s:%d] Semaphore give failed", func, line);
        }
        return result;
    }

    unsigned int Semaphore::getCount() const noexcept
    {
        return mHandle ? uxSemaphoreGetCount(mHandle) : 0;
    }
} // namespace esp32_c3_utils
