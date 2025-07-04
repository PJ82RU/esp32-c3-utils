#include "esp32_c3_utils/sleep_utils.h"
#include <esp32-hal-log.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>

namespace esp32_c3_utils
{
    void deepSleep(const uint64_t sleepDurationMs,
                   const WakeupSource wakeupSrc,
                   const GpioWakeupConfig gpioCfg) noexcept
    {
        // Настройка таймера (если указано время)
        if (sleepDurationMs > 0)
        {
            esp_sleep_enable_timer_wakeup(sleepDurationMs * 1000);
            log_d("Timer wakeup set for %llu ms", sleepDurationMs);
        }

        // Настройка GPIO wakeup (ESP32-C3 использует esp_sleep_enable_gpio_wakeup)
        if (wakeupSrc == WakeupSource::GPIO && gpioCfg.pin != GPIO_NUM_NC)
        {
            if (rtc_gpio_is_valid_gpio(gpioCfg.pin))
            {
                esp_sleep_enable_gpio_wakeup(); // Активируем GPIO wakeup
                gpio_wakeup_enable(gpioCfg.pin,
                                   gpioCfg.activeLow ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
                log_d("GPIO wakeup on pin %d (%s)",
                      gpioCfg.pin,
                      gpioCfg.active_low ? "LOW" : "HIGH");
            }
            else
            {
                log_e("GPIO%d not RTC capable", gpioCfg.pin);
            }
        }

        log_i("Entering deep sleep");
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
} // namespace esp32_c3_utils
