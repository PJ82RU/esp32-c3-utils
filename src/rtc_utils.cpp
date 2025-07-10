#include "esp32_c3_utils/rtc_utils.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "hal/rtc_hal.h"
#include <cstring>

namespace esp32_c3::utils
{
    static constexpr auto TAG = "RTC_UTILS";

    // Доступ к RTC slow memory через HAL API
    static uint8_t* get_rtc_slow_mem() noexcept
    {
        return reinterpret_cast<uint8_t*>(RTC_SLOW_MEM);
    }

    bool writeRtcMemory(const size_t offset, const void* data, const size_t size) noexcept
    {
        if (data == nullptr || size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            ESP_LOGE(TAG, "Invalid write params (offset:%zu, size:%zu)", offset, size);
            return false;
        }

        uint8_t* rtc_mem = get_rtc_slow_mem();
        memcpy(rtc_mem + offset, data, size);
        ESP_LOGD(TAG, "Wrote %zu bytes at offset %zu", size, offset);
        return true;
    }

    bool readRtcMemory(const size_t offset, void* buffer, const size_t size) noexcept
    {
        if (buffer == nullptr || size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            ESP_LOGE(TAG, "Invalid read params (offset:%zu, size:%zu)", offset, size);
            return false;
        }

        const uint8_t* rtc_mem = get_rtc_slow_mem();
        memcpy(buffer, rtc_mem + offset, size);
        ESP_LOGD(TAG, "Read %zu bytes from offset %zu", size, offset);
        return true;
    }

    void clearRtcMemory(const size_t offset, const size_t size) noexcept
    {
        if (size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            ESP_LOGE(TAG, "Invalid clear params (offset:%zu, size:%zu)", offset, size);
            return;
        }

        uint8_t* rtc_mem = get_rtc_slow_mem();
        memset(rtc_mem + offset, 0, size);
        ESP_LOGI(TAG, "Cleared %zu bytes at offset %zu", size, offset);
    }
} // namespace esp32_c3::utils
