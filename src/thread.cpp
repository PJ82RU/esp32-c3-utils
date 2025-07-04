#include "esp32_c3_objects/thread.h"
#include <algorithm>
#include <esp_log.h>

namespace esp32_c3_objects
{
    Thread::Thread(const char* name, const uint32_t stackDepth, const UBaseType_t priority) noexcept
        : mStackDepth(stackDepth),
          mPriority(priority)
    {
        const size_t len = std::min(strlen(name), THREAD_NAME_SIZE - 1);
        memcpy(mName, name, len);
        mName[len] = '\0';
    }

    Thread::~Thread() noexcept
    {
        stop();
    }

    bool Thread::start(const TaskFunction_t taskFunc, void* params) noexcept
    {
        if (mHandle) return false;

        if (xTaskCreate(taskFunc, mName, mStackDepth, params, mPriority, &mHandle) == pdPASS)
        {
            log_i("Task %s created", mName);
            return true;
        }

        log_e("Failed to create task %s", mName);
        return false;
    }

    bool Thread::start(const TaskFunction_t taskFunc, void* params, const BaseType_t coreId) noexcept
    {
        if (mHandle) return false;

        if (xTaskCreatePinnedToCore(taskFunc, mName, mStackDepth, params, mPriority, &mHandle, coreId) == pdPASS)
        {
            log_i("Task %s created on core %d", mName, coreId);
            return true;
        }

        log_e("Failed to create task %s on core %d", mName, coreId);
        return false;
    }

    void Thread::stop() noexcept
    {
        if (mHandle)
        {
            vTaskDelete(mHandle);
            mHandle = nullptr;
            log_i("Task %s deleted", mName);
        }
    }

    bool Thread::isRunning() const noexcept
    {
        return mHandle != nullptr;
    }

    void Thread::suspend() const noexcept
    {
        if (mHandle)
        {
            vTaskSuspend(mHandle);
            log_i("Task %s suspended", mName);
        }
    }

    void Thread::resume() const noexcept
    {
        if (mHandle)
        {
            vTaskResume(mHandle);
            log_i("Task %s resumed", mName);
        }
    }

    uint32_t Thread::stackSize() const noexcept
    {
        return mStackDepth;
    }

    UBaseType_t Thread::stackHighWaterMark() const noexcept
    {
        return mHandle ? uxTaskGetStackHighWaterMark(mHandle) : 0;
    }
} // namespace esp32_c3_utils
