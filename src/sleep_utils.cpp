#include "esp32_c3_utils/sleep_utils.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"

namespace esp32_c3::utils
{
    void deepSleep(const uint64_t sleepDurationMs,
                   const WakeupSource wakeupSrc,
                   const GpioWakeupConfig gpioCfg) noexcept
    {
        // Настройка пробуждения по таймеру
        if (sleepDurationMs > 0)
        {
            esp_sleep_enable_timer_wakeup(sleepDurationMs * 1000);
            ESP_LOGD("Sleep", "Timer wakeup set for %llu ms", sleepDurationMs);
        }

        // Настройка пробуждения по GPIO
        if (wakeupSrc == WakeupSource::GPIO && gpioCfg.pin != GPIO_NUM_NC)
        {
            if (rtc_gpio_is_valid_gpio(gpioCfg.pin))
            {
                esp_sleep_enable_gpio_wakeup();
                gpio_wakeup_enable(gpioCfg.pin,
                                   gpioCfg.activeLow ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
                ESP_LOGD("Sleep", "GPIO wakeup on pin %d (%s)",
                         gpioCfg.pin,
                         gpioCfg.activeLow ? "LOW" : "HIGH");
            }
            else
            {
                ESP_LOGE("Sleep", "GPIO%d is not RTC capable", gpioCfg.pin);
            }
        }

        ESP_LOGI("Sleep", "Entering deep sleep");
        esp_deep_sleep_start();
    }

    WakeupSource getWakeupCause() noexcept
    {
        switch (esp_sleep_get_wakeup_cause())
        {
        case ESP_SLEEP_WAKEUP_TIMER: return WakeupSource::TIMER;
        case ESP_SLEEP_WAKEUP_GPIO: return WakeupSource::GPIO;
        case ESP_SLEEP_WAKEUP_ULP: return WakeupSource::ULP;
        default: return WakeupSource::TIMER;
        }
    }
} // namespace esp32_c3::utils
