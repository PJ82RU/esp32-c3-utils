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
        mQueue(bufferSize, sizeof(BufferItem)),
        mNumItems(numCallbacks),
        mItems(numCallbacks > 0 ? std::make_unique<Item[]>(numCallbacks) : nullptr),
        mBufferSize(bufferSize),
        mBuffer(bufferSize > 0 ? std::make_unique<T[]>(bufferSize) : nullptr)
    {
        if (mBuffer && mItems)
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
        return mBuffer && mItems && mThread.isRunning();
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
        if (!isInitialized() || !input)
        {
            ESP_LOGE(TAG, "Invoke failed: not initialized or null input");
            return;
        }

        std::lock_guard lock(mMutex);

        // Копируем входные данные в буфер
        const uint8_t bufferIndex = mCurrentBufferIndex;
        mBuffer[bufferIndex] = *input;
        mCurrentBufferIndex = (bufferIndex + 1) % mBufferSize;

        // Формируем задание для обработки
        const BufferItem item{
            .itemIndex = index,
            .bufferIndex = bufferIndex,
            .response = response
        };

        if (mQueue.send(&item) != pdTRUE)
        {
            ESP_LOGE(TAG, "Failed to send item to queue");
        }
    }

    template <typename T, typename P>
    bool Callback<T, P>::read(T* value) const noexcept
    {
        if (!isInitialized() || !value) return false;

        BufferItem item{};
        const bool result = mQueue.receive(&item, 0);
        if (result)
        {
            std::lock_guard lock(mMutex);
            *value = mBuffer[item.bufferIndex];
        }

        return result;
    }

    template <typename T, typename P>
    void Callback<T, P>::callbackTask(void* arg) noexcept
    {
        if (const auto* cb = static_cast<Callback*>(arg))
        {
            cb->run();
        }
        vTaskDelete(nullptr);
    }

    // ReSharper disable CppDFAUnusedValue
    template <typename T, typename P>
    void Callback<T, P>::process(const BufferItem& item) const noexcept
    {
        const T* input = &mBuffer[item.bufferIndex];
        std::lock_guard lock(mMutex);

        for (uint8_t i = 0; i < mNumItems; ++i)
        {
            if (const auto& [onlyIndex, func, params] = mItems[i]; func && (!onlyIndex || item.itemIndex == i))
            {
                if (item.response)
                {
                    // Выделяем временный буфер
                    T output;
                    if (func(input, &output, params))
                    {
                        item.response->invoke(&output);
                    }
                }
                else
                {
                    (void)func(input, nullptr, params);
                }
            }
        }
    }

    template <typename T, typename P>
    void Callback<T, P>::run() const noexcept
    {
        BufferItem item{};
        while (mQueue.receive(&item, portMAX_DELAY))
        {
            processItems(item);
        }
    }
} // namespace esp32_c3::objects
