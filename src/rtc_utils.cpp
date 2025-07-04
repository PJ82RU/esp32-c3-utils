#include "esp32_c3_utils/rtc_utils.h"
#include <cstring>
#include <esp32-hal-log.h>
#include <esp_system.h>

// Для ESP32-C3 используем RTC slow memory через макросы
extern "C" {
extern uint8_t* rtc_slow_mem;
#define RTC_SLOW_MEM ((uint8_t*)rtc_slow_mem)
}

namespace esp32_c3_utils
{
    bool writeRtcMemory(const size_t offset, const void* data, const size_t size) noexcept
    {
        if (!data || size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            log_e("Invalid RTC write params (offset:%zu, size:%zu)", offset, size);
            return false;
        }

        memcpy(RTC_SLOW_MEM + offset, data, size);
        log_d("Wrote %zu bytes to RTC at offset %zu", size, offset);
        return true;
    }

    bool readRtcMemory(const size_t offset, void* buffer, const size_t size) noexcept
    {
        if (!buffer || size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            log_e("Invalid RTC read params (offset:%zu, size:%zu)", offset, size);
            return false;
        }

        memcpy(buffer, RTC_SLOW_MEM + offset, size);
        log_d("Read %zu bytes from RTC at offset %zu", size, offset);
        return true;
    }

    void clearRtcMemory(const size_t offset, const size_t size) noexcept
    {
        if (size == 0 || offset + size > RTC_SLOW_MEM_SIZE)
        {
            log_e("Invalid RTC clear params (offset:%zu, size:%zu)", offset, size);
            return;
        }

        memset(RTC_SLOW_MEM + offset, 0, size);
        log_i("Cleared %zu bytes in RTC at offset %zu", size, offset);
    }
} // namespace esp32_c3_utils
