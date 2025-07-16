#include "esp32_c3_objects/callback.h"
#include <esp_log.h>

namespace esp32_c3::objects
{
    Callback::Callback(const uint8_t bufferSize,
                       const size_t itemSize,
                       const uint8_t numCallbacks,
                       const char* name,
                       const uint32_t stackDepth,
                       const UBaseType_t priority) noexcept :
        mThread(name, stackDepth, priority),
        mQueue(bufferSize, sizeof(BufferItem)),
        mNumItems(numCallbacks),
        mItems((numCallbacks > 0) ? new(std::nothrow) Item[numCallbacks] : nullptr),
        mBufferSize(bufferSize),
        mItemSize(itemSize),
        mBuffer((bufferSize > 0 && itemSize > 0 && bufferSize < SIZE_MAX / itemSize)
                    ? new(std::nothrow) uint8_t[bufferSize * itemSize]
                    : nullptr)
    {
        if (mBuffer && mItems)
        {
            ESP_LOGI(TAG, "Constructed with buffer %dx%d and %d callbacks", bufferSize, itemSize, numCallbacks);
            free(); // Инициализируем нулями

            if (!mThread.start(&Callback::callbackTask, this))
            {
                ESP_LOGE(TAG, "Failed to start callback thread");
            }
        }
        else
        {
            ESP_LOGE(TAG, "Memory allocation failed in constructor");
        }
    }

    Callback::~Callback()
    {
        mThread.stop();
        if (mItems) delete[] mItems;
        if (mBuffer) delete[] mBuffer;
        ESP_LOGI(TAG, "Callback destroyed");
    }

    bool Callback::isInitialized() const noexcept
    {
        std::lock_guard lock(mMutex);
        return mBuffer && mItems && mThread.isRunning();
    }

    int16_t Callback::addCallback(const EventSendFunc func, void* params, const bool onlyIndex) const noexcept
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

    void Callback::free() const noexcept
    {
        if (isInitialized())
        {
            std::lock_guard lock(mMutex);
            std::memset(mItems, 0, sizeof(Item) * mNumItems);
            mLastFreeIndex = 0;
            ESP_LOGD(TAG, "Cleared all callbacks");
        }
    }

    void Callback::invoke(const void* value, const int16_t index) noexcept
    {
        if (!isInitialized() || !value) return;

        std::lock_guard lock(mMutex);

        const uint8_t bufferIndex = mCurrentBufferIndex;
        std::memcpy(&mBuffer[bufferIndex * mItemSize], value, mItemSize);
        mCurrentBufferIndex = (bufferIndex + 1) % mBufferSize;

        const BufferItem item{index, bufferIndex};
        (void)mQueue.send(&item);
    }

    bool Callback::read(void* value) const noexcept
    {
        if (!isInitialized() || !value) return false;

        BufferItem item{};
        const bool result = mQueue.receive(&item, 0);
        if (result)
        {
            std::lock_guard lock(mMutex);
            std::memcpy(value, &mBuffer[item.bufferIndex * mItemSize], mItemSize);
        }

        return result;
    }

    void Callback::callbackTask(void* arg) noexcept
    {
        if (const auto* cb = static_cast<Callback*>(arg))
        {
            cb->run();
        }
        vTaskDelete(nullptr);
    }

    void Callback::processItems(const BufferItem& item) const noexcept
    {
        const size_t pos = item.bufferIndex * mItemSize;
        std::lock_guard lock(mMutex);

        for (uint8_t i = 0; i < mNumItems; ++i)
        {
            if (const auto& [onlyIndex, func, params] = mItems[i]; func && (!onlyIndex || item.itemIndex == i))
            {
                if (void* currentData = &mBuffer[pos]; func(currentData, params))
                {
                    parentCallback.invoke(currentData);
                }
            }
        }
    }

    void Callback::run() const noexcept
    {
        BufferItem item{};
        while (mQueue.receive(&item, portMAX_DELAY))
        {
            processItems(item);
        }
    }
} // namespace esp32_c3::objects
