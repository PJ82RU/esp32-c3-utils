#include "esp32_c3_objects/thread.h"

#include <algorithm>
#include <esp_err.h>
#include <esp_log.h>

namespace esp32_c3::objects
{
    Thread::Thread(const std::string_view name, const uint32_t stackDepth, const UBaseType_t priority) noexcept
        : mStackDepth(stackDepth),
          mPriority(priority),
          mStackWarningThreshold(stackDepth / 10)
    {
        const size_t copySize = std::min(name.size(), THREAD_NAME_SIZE - 1);
        std::copy_n(name.begin(), copySize, mName.begin());
        mName[copySize] = '\0';

        if (shouldMonitorStack())
        {
            ESP_LOGI(TAG, "[%.*s] Stack monitoring active (threshold: %lu words)",
                     static_cast<int>(name.size()), name.data(),
                     (unsigned long)mStackWarningThreshold);
        }
    }

    Thread::~Thread() noexcept
    {
        stop(false);
    }

    esp_err_t Thread::start(LoopFunc loopFunc, const uint32_t intervalMs, const bool startPaused) noexcept
    {
        TaskHandle_t handle = nullptr;
        if (mHandle.load())
        {
            ESP_LOGE(TAG, "Task %s already running", mName.data());
            return ESP_ERR_INVALID_STATE;
        }

        mLoopContext.reset(new LoopContext(
            std::move(loopFunc),
            pdMS_TO_TICKS(intervalMs),
            this,
            false,
            startPaused
        ));

        if (xTaskCreate(loopWrapper, mName.data(), mStackDepth, mLoopContext.get(), mPriority, &handle) != pdPASS)
        {
            mLoopContext.reset();
            ESP_LOGE(TAG, "Failed to create loop task %s", mName.data());
            return ESP_ERR_NO_MEM;
        }
        mHandle.store(handle);

        ESP_LOGI(TAG, "Loop task %s started (interval: %" PRIu32 "ms)", mName.data(), intervalMs);
        return ESP_OK;
    }

    esp_err_t Thread::start(const TaskFunction_t taskFunc, void* params) noexcept
    {
        TaskHandle_t handle = nullptr;
        if (mHandle.load())
        {
            ESP_LOGE(TAG, "Task %s already running", mName.data());
            return ESP_ERR_INVALID_STATE;
        }

        if (xTaskCreate(taskFunc, mName.data(), mStackDepth, params, mPriority, &handle) == pdPASS)
        {
            mHandle.store(handle);
            ESP_LOGI(TAG, "Task %s created", mName.data());
            return ESP_OK;
        }

        ESP_LOGE(TAG, "Failed to create task %s", mName.data());
        return ESP_ERR_NO_MEM;
    }

    esp_err_t Thread::start(const TaskFunction_t taskFunc, void* params, const BaseType_t coreId) noexcept
    {
        TaskHandle_t handle = nullptr;
        if (coreId < 0 || coreId >= portNUM_PROCESSORS)
        {
            ESP_LOGE(TAG, "Invalid core ID: %d", coreId);
            return ESP_ERR_INVALID_ARG;
        }

        if (mHandle.load())
        {
            ESP_LOGE(TAG, "Task %s already running", mName.data());
            return ESP_ERR_INVALID_STATE;
        }

        if (xTaskCreatePinnedToCore(
            taskFunc,
            mName.data(),
            mStackDepth,
            params,
            mPriority,
            &handle,
            coreId
        ) == pdPASS)
        {
            mHandle.store(handle);
            ESP_LOGI(TAG, "Task %s created on core %d", mName.data(), coreId);
            return ESP_OK;
        }

        ESP_LOGE(TAG, "Failed to create task %s on core %d", mName.data(), coreId);
        return ESP_ERR_NO_MEM;
    }

    bool Thread::quickStart(const LoopFunc& loopFunc)
    {
        if (state() != State::NOT_RUNNING) return true;
        if (start(loopFunc) != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to start callback thread");
            return false;
        }
        return true;
    }

    // ReSharper disable CppDFAConstantConditions
    void Thread::stop(const bool softStop) noexcept
    {
        const TaskHandle_t handle = mHandle.exchange(nullptr);
        if (!handle) return;

        if (softStop && mLoopContext)
        {
            mLoopContext->shouldStop = true;

            constexpr TickType_t timeoutStep = pdMS_TO_TICKS(10);
            const TickType_t end = xTaskGetTickCount() + pdMS_TO_TICKS(1000);

            while (handle && end > xTaskGetTickCount())
            {
                vTaskDelay(timeoutStep);
                // Проверяем, не был ли хендл уже удален в другом потоке
                if (mHandle.load() == nullptr)
                {
                    return;
                }
            }

            if (handle)
            {
                ESP_LOGW(TAG, "Task %s didn't stop gracefully, forcing stop", mName.data());
                vTaskDelete(handle);
            }
        }
        else
        {
            vTaskDelete(handle);
            ESP_LOGI(TAG, "Task %s forcefully deleted", mName.data());
        }
    }

    Thread::State Thread::state() const noexcept
    {
        const TaskHandle_t handle = mHandle.load();
        if (!handle)
        {
            return State::NOT_RUNNING;
        }

        switch (eTaskGetState(handle))
        {
        case eReady: return State::READY;
        case eRunning: return State::RUNNING;
        case eBlocked: return State::SUSPENDED; // В FreeRTOS eBlocked часто означает ожидание
        case eSuspended: return State::SUSPENDED;
        default: return State::NOT_RUNNING;
        }
    }

    void Thread::suspend() const noexcept
    {
        if (const TaskHandle_t handle = mHandle.load())
        {
            vTaskSuspend(handle);
            ESP_LOGI(TAG, "Task %s suspended", mName.data());
        }
    }

    void Thread::resume() const noexcept
    {
        if (const TaskHandle_t handle = mHandle.load())
        {
            vTaskResume(handle);
            ESP_LOGI(TAG, "Task %s resumed", mName.data());
        }
    }

    esp_err_t Thread::setPriority(const UBaseType_t newPriority) noexcept
    {
        const TaskHandle_t handle = mHandle.load();
        if (!handle)
        {
            return ESP_ERR_INVALID_STATE;
        }

        vTaskPrioritySet(handle, newPriority);
        mPriority = newPriority;
        ESP_LOGI(TAG, "Task %s priority changed to %d", mName.data(), newPriority);
        return ESP_OK;
    }

    uint32_t Thread::stackSize() const noexcept
    {
        return mStackDepth;
    }

    UBaseType_t Thread::stackHighWaterMark() const noexcept
    {
        const TaskHandle_t handle = mHandle.load();
        return handle ? uxTaskGetStackHighWaterMark(handle) : 0;
    }

    bool Thread::shouldMonitorStack() noexcept
    {
        return esp_log_level_get(TAG) >= ESP_LOG_ERROR;
    }

    void Thread::checkStack() const noexcept
    {
        if (!shouldMonitorStack()) return;

        const TaskHandle_t handle = mHandle.load();
        if (!handle) return;

        if (const UBaseType_t free = uxTaskGetStackHighWaterMark(handle); free < mStackWarningThreshold)
        {
            ESP_LOGW(TAG, "[%s] Low stack: %lu/%lu words (%.1f%%)",
                     mName.data(),
                     (unsigned long)free,
                     (unsigned long)mStackDepth,
                     (free * 100.0f) / mStackDepth);
        }
    }

    const char* Thread::name() const noexcept
    {
        return mName.data();
    }

    void Thread::loopWrapper(void* arg) noexcept
    {
        if (!arg)
        {
            ESP_LOGE(TAG, "Null context in loop wrapper");
            vTaskDelete(nullptr);
            return;
        }

        auto* ctx = static_cast<LoopContext*>(arg);
        if (ctx->isStartPaused.load(std::memory_order_acquire))
        {
            ctx->thread->suspend();
        }

        while (!ctx->shouldStop.load(std::memory_order_relaxed))
        {
            // Автоматическая проверка стека
            ctx->thread->checkStack();

            const auto action = ctx->func();

            if (action == LoopAction::CONTINUE)
            {
                vTaskDelay(ctx->interval);
                continue;
            }

            if (action == LoopAction::PAUSE)
            {
                ctx->thread->suspend();
                continue;
            }

            if (action == LoopAction::STOP)
            {
                ctx->shouldStop.store(true, std::memory_order_release);
                break;
            }
        }

        ctx->thread->mHandle = nullptr;
        vTaskDelete(nullptr);
    }
} // namespace esp32_c3::objects
