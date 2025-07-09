#include "esp32_c3_objects/thread.h"
#include <algorithm>

namespace esp32_c3::objects
{
    Thread::Thread(const std::string_view name, const uint32_t stackDepth, const UBaseType_t priority) noexcept
        : mStackDepth(stackDepth),
          mPriority(priority)
    {
        const size_t copySize = std::min(name.size(), THREAD_NAME_SIZE - 1);
        std::copy_n(name.begin(), copySize, mName.begin());
        mName[copySize] = '\0';
    }

    Thread::~Thread() noexcept
    {
        stop();
    }

    bool Thread::start(const TaskFunction_t taskFunc, void* params) noexcept
    {
        if (mHandle)
        {
            ESP_LOGE(TAG, "Task %s already running", mName.data());
            return false;
        }

        if (xTaskCreate(taskFunc, mName.data(), mStackDepth, params, mPriority, &mHandle) == pdPASS)
        {
            ESP_LOGI(TAG, "Task %s created", mName.data());
            return true;
        }

        ESP_LOGE(TAG, "Failed to create task %s", mName.data());
        return false;
    }

    bool Thread::start(const TaskFunction_t taskFunc, void* params, const BaseType_t coreId) noexcept
    {
        if (mHandle)
        {
            ESP_LOGE(TAG, "Task %s already running", mName.data());
            return false;
        }

        if (xTaskCreatePinnedToCore(
            taskFunc,
            mName.data(),
            mStackDepth,
            params,
            mPriority,
            &mHandle,
            coreId
        ) == pdPASS)
        {
            ESP_LOGI(TAG, "Task %s created on core %d", mName.data(), coreId);
            return true;
        }

        ESP_LOGE(TAG, "Failed to create task %s on core %d", mName.data(), coreId);
        return false;
    }

    void Thread::stop() noexcept
    {
        if (mHandle)
        {
            vTaskDelete(mHandle);
            mHandle = nullptr;
            ESP_LOGI(TAG, "Task %s deleted", mName.data());
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
            ESP_LOGI(TAG, "Task %s suspended", mName.data());
        }
    }

    void Thread::resume() const noexcept
    {
        if (mHandle)
        {
            vTaskResume(mHandle);
            ESP_LOGI(TAG, "Task %s resumed", mName.data());
        }
    }

    UBaseType_t Thread::stackHighWaterMark() const noexcept
    {
        return mHandle ? uxTaskGetStackHighWaterMark(mHandle) : 0;
    }
} // namespace esp32_c3::objects
