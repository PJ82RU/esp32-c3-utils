// ReSharper disable CppDFAUnreachableCode
#include "esp32_c3_utils/power_utils.h"
#include "esp_pm.h"
#include "driver/adc.h"
#include "esp_sleep.h"
#include "esp_log.h"

namespace esp32_c3::utils
{
    bool setCpuFrequency(CpuFrequency freq) noexcept
    {
        if (freq < CpuFrequency::MIN || freq > CpuFrequency::MAX)
        {
            ESP_LOGE("Power", "Invalid CPU frequency: %d", static_cast<int>(freq));
            return false;
        }

        const esp_pm_config_t pmConfig = {
            .max_freq_mhz = static_cast<int>(freq),
            .min_freq_mhz = static_cast<int>(freq),
            .light_sleep_enable = (freq == CpuFrequency::MHz_80)
        };

        if (const esp_err_t err = esp_pm_configure(&pmConfig); err != ESP_OK)
        {
            ESP_LOGE("Power", "Failed to set CPU freq: %s", esp_err_to_name(err));
            return false;
        }

        ESP_LOGI("Power", "CPU frequency set to %d MHz", static_cast<int>(freq));
        return true;
    }

    float readBatteryVoltage(const gpio_num_t adcPin, const float voltageDivider) noexcept
    {
        if (adcPin != GPIO_NUM_0)
        {
            ESP_LOGE("Power", "Only GPIO0 supported for ADC on ESP32-C3");
            return 0.0f;
        }

        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);

        const float voltage = static_cast<float>(adc1_get_raw(ADC1_CHANNEL_0)) * 3.3f / 4095.0f;
        const float result = voltage * voltageDivider;

        ESP_LOGD("Power", "Battery voltage: %.2fV (raw: %.2fV)", result, voltage);
        return result;
    }

    void enterPowerSave(const PowerSaveMode mode, const uint32_t durationMs) noexcept
    {
        switch (mode)
        {
        case PowerSaveMode::LIGHT_SLEEP:
            if (durationMs > 0)
            {
                esp_sleep_enable_timer_wakeup(durationMs * 1000);
            }
            esp_light_sleep_start();
            break;

        case PowerSaveMode::DEEP_SLEEP:
            if (durationMs > 0)
            {
                esp_sleep_enable_timer_wakeup(durationMs * 1000);
            }
            esp_deep_sleep_start();
            break;

        case PowerSaveMode::OFF:
            gpio_hold_en(GPIO_NUM_0);
            esp_deep_sleep(0);
            break;

        default:
            ESP_LOGE("Power", "Unknown power save mode");
        }
    }
} // namespace esp32_c3::utils
