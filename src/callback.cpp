#include "esp32_c3_objects/callback.h"
#include <esp_log.h>

namespace esp32_c3::objects
{
    // ReSharper disable CppDFAUnreachableCode
    template <typename T, typename P>
    Callback<T, P>::Callback(const char* name,
                             const uint8_t bufferSize,
                             const uint8_t numCallbacks,
                             const uint32_t stackDepth,
                             const UBaseType_t priority) noexcept :
        mThread(name, stackDepth, priority),
        mQueue(bufferSize),
        mItems(numCallbacks > 0 ? std::make_unique<Item[]>(numCallbacks) : nullptr),
        mNumItems(numCallbacks)
    {
        if (mQueue.isValid() && mItems)
        {
            ESP_LOGI(TAG, "Constructed with buffer size %d and %d callbacks",
                     bufferSize, numCallbacks);
            free();

            if (!mThread.start(&Callback<T, P>::callbackTask, this))
            {
                ESP_LOGE(TAG, "Failed to start callback thread");
            }
        }
        else
        {
            ESP_LOGE(TAG, "Memory allocation failed");
        }
    }

    template <typename T, typename P>
    Callback<T, P>::~Callback()
    {
        mThread.stop();
        ESP_LOGI(TAG, "Callback destroyed");
    }

    template <typename T, typename P>
    bool Callback<T, P>::isInitialized() const noexcept
    {
        std::lock_guard lock(mMutex);
        return mQueue.isValid() && mItems && mThread.isRunning();
    }

    template <typename T, typename P>
    int16_t Callback<T, P>::addCallback(const CallbackFunction func, P params, const bool onlyIndex) const noexcept
    {
        if (!isInitialized() || !func) return -1;

        std::lock_guard lock(mMutex);

        // Быстрый поиск начиная с последнего свободного индекса
        for (int16_t i = 0; i < mNumItems; ++i)
        {
            if (const int16_t currentIndex = (mLastFreeIndex + i) % mNumItems; !mItems[currentIndex].func)
            {
                mItems[currentIndex] = {onlyIndex, func, params};
                mLastFreeIndex = (currentIndex + 1) % mNumItems;

                ESP_LOGD(TAG, "Added callback at index %d", currentIndex);
                return currentIndex;
            }
        }

        ESP_LOGW(TAG, "No free slots for callback");
        return -1;
    }

    template <typename T, typename P>
    void Callback<T, P>::free() const noexcept
    {
        if (isInitialized())
        {
            std::lock_guard lock(mMutex);
            if (mItems)
            {
                std::memset(mItems.get(), 0, sizeof(Item) * mNumItems);
            }
            mLastFreeIndex = 0;
            ESP_LOGD(TAG, "Cleared all callbacks");
        }
    }

    template <typename T, typename P>
    void Callback<T, P>::invoke(const T* input, SimpleCallback<T>* response, const int16_t index) noexcept
    {
        if (!input || !isInitialized())
        {
            ESP_LOGE(TAG, "Invoke failed: not initialized or null input");
            return;
        }

        TaskItem item{index, *input, response};
        if (!mQueue.send(&item))
        {
            ESP_LOGE(TAG, "Failed to send item to queue");
        }
    }

    template <typename T, typename P>
    bool Callback<T, P>::read(T* value) const noexcept
    {
        if (value && isInitialized())
        {
            if (TaskItem item; mQueue.receive(item))
            {
                *value = item.data;
                return true;
            }
        }
        return false;
    }

    template <typename T, typename P>
    void Callback<T, P>::callbackTask(void* arg) noexcept
    {
        static_cast<Callback*>(arg)->run();
        vTaskDelete(nullptr);
    }

    // ReSharper disable CppDFAUnusedValue
    template <typename T, typename P>
    void Callback<T, P>::process(const TaskItem& item) const noexcept
    {
        std::lock_guard lock(mMutex);
        for (uint8_t i = 0; i < mNumItems; ++i)
        {
            if (const auto& cb = mItems[i]; cb.func && (!cb.onlyIndex || item.itemIndex == i))
            {
                if (item.response)
                {
                    T output;
                    if (cb.func(&item.data, &output, cb.params))
                    {
                        item.response->invoke(&output);
                    }
                }
                else
                {
                    cb.func(&item.data, nullptr, cb.params);
                }
            }
        }
    }

    template <typename T, typename P>
    void Callback<T, P>::run() const noexcept
    {
        TaskItem item;
        while (mQueue.receive(item))
        {
            process(item);
        }
    }
} // namespace esp32_c3::objects
