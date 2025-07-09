#include "esp32_c3_utils/rtc_utils.h"
#include "esp_log.h"
#include <cstring>

// Для ESP32-C3 используем RTC slow memory
extern "C" {
extern uint8_t* rtc_slow_mem;
}

namespace esp32_c3::utils
{
    bool writeRtcMemory(const size_t offset, const void* data, const size_t size) noexcept
    {
        if (data == nullptr || size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            ESP_LOGE("RTC", "Invalid write params (offset:%zu, size:%zu)", offset, size);
            return false;
        }

        uint8_t* dest = rtc_slow_mem + offset;
        memcpy(dest, data, size);
        ESP_LOGD("RTC", "Wrote %zu bytes at offset %zu", size, offset);
        return true;
    }

    bool readRtcMemory(const size_t offset, void* buffer, const size_t size) noexcept
    {
        if (buffer == nullptr || size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            ESP_LOGE("RTC", "Invalid read params (offset:%zu, size:%zu)", offset, size);
            return false;
        }

        const uint8_t* src = rtc_slow_mem + offset;
        memcpy(buffer, src, size);
        ESP_LOGD("RTC", "Read %zu bytes from offset %zu", size, offset);
        return true;
    }

    void clearRtcMemory(const size_t offset, const size_t size) noexcept
    {
        if (size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            ESP_LOGE("RTC", "Invalid clear params (offset:%zu, size:%zu)", offset, size);
            return;
        }

        uint8_t* dest = rtc_slow_mem + offset;
        memset(dest, 0, size);
        ESP_LOGI("RTC", "Cleared %zu bytes at offset %zu", size, offset);
    }
} // namespace esp32_c3::utils
