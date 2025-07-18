#include "esp32_c3_objects/simple_callback.h"

namespace esp32_c3::objects
{
    template <typename T, typename P>
    SimpleCallback<T, P>::SimpleCallback(const CallbackFunction callback, P params) noexcept
        : mCallback(callback),
          mParams(params)
    {
    }

    template <typename T, typename P>
    void SimpleCallback<T, P>::set(CallbackFunction callback, P params) noexcept
    {
        mCallback = callback;
        mParams = params;
    }

    template <typename T, typename P>
    void SimpleCallback<T, P>::reset() noexcept
    {
        mCallback = nullptr;
        mParams = nullptr;
    }

    template <typename T, typename P>
    void SimpleCallback<T, P>::invoke(T* value) const noexcept
    {
        if (mCallback != nullptr)
        {
            mCallback(value, mParams);
        }
    }

    template <typename T, typename P>
    bool SimpleCallback<T, P>::isSet() const noexcept
    {
        return mCallback != nullptr;
    }
} // namespace esp32_c3::utils
