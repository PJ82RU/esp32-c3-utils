#include "esp32_c3_objects/callback.h"
#include <cstring>

namespace esp32_c3_objects
{
    Callback::Callback(const uint8_t bufferSize,
                       const size_t itemSize,
                       const char* name,
                       const uint32_t stackDepth,
                       const UBaseType_t priority) :
        mThread(name, stackDepth, priority),
        mQueue(bufferSize, sizeof(BufferItem)),
        mSemaphore(false)
    {
        if (bufferSize > 0 && itemSize > 0)
        {
            mBufferSize = bufferSize;
            mItemSize = itemSize;
            mBuffer = new(std::nothrow) uint8_t[mBufferSize * mItemSize];

            if (mBuffer)
            {
                log_i("Initialized with buffer %dx%d", mBufferSize, mItemSize);
                return;
            }
        }
        log_e("Invalid initialization parameters");
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

        if (mSemaphore.take())
        {
            for (int16_t i = 0; i < mNumItems; ++i)
            {
                if (!mItems[i].func)
                {
                    mItems[i] = {onlyIndex, func, params};
                    (void)mSemaphore.give();
                    log_d("Added callback at index %d", i);
                    return i;
                }
            }
            (void)mSemaphore.give();
        }

        log_w("No free slots for callback");
        return -1;
    }

    void Callback::free() const noexcept
    {
        if (isInitialized() && mSemaphore.take())
        {
            std::memset(mItems, 0, sizeof(Item) * mNumItems);
            (void)mSemaphore.give();
            log_d("Cleared all callbacks");
        }
    }

    void Callback::invoke(const void* value, const int16_t index) noexcept
    {
        if (!mBuffer || !value || !mSemaphore.take()) return;

        std::memcpy(&mBuffer[mCurrentBufferIndex * mItemSize], value, mItemSize);

        const BufferItem item{index, mCurrentBufferIndex};
        mCurrentBufferIndex = (mCurrentBufferIndex + 1) % mBufferSize;

        (void)mQueue.send(&item);
        (void)mSemaphore.give();
    }

    bool Callback::read(void* value) const noexcept
    {
        if (!mBuffer || !value || !mSemaphore.take()) return false;

        BufferItem item{};
        const bool result = mQueue.receive(&item, 0);
        if (result)
        {
            std::memcpy(value, &mBuffer[item.bufferIndex * mItemSize], mItemSize);
        }

        (void)mSemaphore.give();
        return result;
    }

    void Callback::callbackTask(void* arg) noexcept
    {
        const auto* cb = static_cast<Callback*>(arg);
        if (cb)
        {
            cb->run();
        }
    }

    void Callback::processItems(const BufferItem& item) const noexcept
    {
        const size_t pos = item.bufferIndex * mItemSize;

        if (mSemaphore.take())
        {
            for (uint8_t i = 0; i < mNumItems; ++i)
            {
                const Item& it = mItems[i];
                if (it.func && (!it.onlyIndex || it.onlyIndex && item.itemIndex == i))
                {
                    if (it.func(&mBuffer[pos], it.params))
                    {
                        parentCallback.invoke(&mBuffer[pos]);
                    }
                }
            }
            (void)mSemaphore.give();
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
} // namespace esp32_c3_utils
