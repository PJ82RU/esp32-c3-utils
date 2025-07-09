#include "esp32_c3_objects/callback.h"
#include <esp_log.h>

namespace esp32_c3::objects
{
    Callback::Callback(const uint8_t bufferSize,
                       const size_t itemSize,
                       const char* name,
                       const uint32_t stackDepth,
                       const UBaseType_t priority) noexcept :
        mThread(name, stackDepth, priority),
        mQueue(bufferSize, sizeof(BufferItem)),
        mBufferSize(bufferSize),
        mItemSize(itemSize),
        mBuffer((bufferSize > 0 && itemSize > 0 && bufferSize < SIZE_MAX / itemSize)
                    ? new(std::nothrow) uint8_t[bufferSize * itemSize]
                    : nullptr)
    {
        if (mBuffer)
        {
            ESP_LOGI(TAG, "Initialized with buffer %dx%d", mBufferSize, mItemSize);
        }
        else if (bufferSize > 0 && itemSize > 0)
        {
            ESP_LOGE(TAG, "Memory allocation failed");
        }
    }

    Callback::~Callback()
    {
        mThread.stop();
        delete[] mItems;
        delete[] mBuffer;
    }

    bool Callback::init(const uint8_t numCallbacks) noexcept
    {
        if (mBuffer && mNumItems == 0 && numCallbacks > 0)
        {
            mItems = new(std::nothrow) Item[numCallbacks];
            if (mItems)
            {
                mNumItems = numCallbacks;
                free();
                return mThread.start(&Callback::callbackTask, this);
            }
        }
        return false;
    }

    bool Callback::isInitialized() const noexcept
    {
        return mBuffer && mItems;
    }

    int16_t Callback::addCallback(const EventSendFunc func, void* params, const bool onlyIndex) const noexcept
    {
        if (!isInitialized() || !func) return -1;

        std::lock_guard lock(mMutex);
        for (int16_t i = 0; i < mNumItems; ++i)
        {
            if (!mItems[i].func)
            {
                mItems[i] = {onlyIndex, func, params};
                ESP_LOGD(TAG, "Added callback at index %d", i);
                return i;
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
            ESP_LOGD(TAG, "Cleared all callbacks");
        }
    }

    void Callback::invoke(const void* value, const int16_t index) noexcept
    {
        if (!mBuffer || !value) return;

        {
            std::lock_guard lock(mMutex);
            std::memcpy(&mBuffer[mCurrentBufferIndex * mItemSize], value, mItemSize);

            const BufferItem item{index, mCurrentBufferIndex};
            mCurrentBufferIndex = (mCurrentBufferIndex + 1) % mBufferSize;

            (void)mQueue.send(&item);
        }
    }

    bool Callback::read(void* value) const noexcept
    {
        if (!mBuffer || !value) return false;

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
