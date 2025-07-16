#include "esp32_c3_utils/power_utils.h"
#include "esp_pm.h"
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

        adc_oneshot_unit_handle_t adc_handle = nullptr;
        float result = 0.0f;

        // 1. Инициализация ADC
        constexpr adc_oneshot_unit_init_cfg_t init_config = {
            .unit_id = ADC_UNIT_1,
            .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
            .ulp_mode = ADC_ULP_MODE_DISABLE,
        };

        esp_err_t err = adc_oneshot_new_unit(&init_config, &adc_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE("Power", "ADC init failed: %s", esp_err_to_name(err));
            return result;
        }

        // 2. Конфигурация канала ADC
        constexpr adc_oneshot_chan_cfg_t config = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12,
        };

        err = adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &config);
        if (err == ESP_OK)
        {
            // 3. Получение сырого значения
            int raw_value;
            err = adc_oneshot_read(adc_handle, ADC_CHANNEL_0, &raw_value);
            if (err == ESP_OK)
            {
                // 4. Конвертация в напряжение
                const float voltage = static_cast<float>(raw_value) * 3.3f / 4095.0f;
                result = voltage * voltageDivider;
                ESP_LOGD("Power", "Battery voltage: %.2fV (raw: %.2fV)", result, voltage);
            }
            else
            {
                ESP_LOGE("Power", "ADC read failed: %s", esp_err_to_name(err));
            }
        }
        else
        {
            ESP_LOGE("Power", "ADC config failed: %s", esp_err_to_name(err));
        }

        adc_oneshot_del_unit(adc_handle);
        return result;
    }

    // ReSharper disable CppDFAUnreachableCode
    void enterPowerSave(const PowerSaveMode mode, const uint32_t durationMs) noexcept
    {
        esp_err_t err = ESP_OK;

        switch (mode)
        {
        case PowerSaveMode::LIGHT_SLEEP:
            if (durationMs > 0)
            {
                err = esp_sleep_enable_timer_wakeup(durationMs * 1000);
                if (err != ESP_OK)
                {
                    ESP_LOGE("Power", "Timer wakeup config failed: %s", esp_err_to_name(err));
                    return;
                }
            }
            esp_light_sleep_start();
            break;


        case PowerSaveMode::DEEP_SLEEP:
            if (durationMs > 0)
            {
                err = esp_sleep_enable_timer_wakeup(durationMs * 1000);
                if (err != ESP_OK)
                {
                    ESP_LOGE("Power", "Timer wakeup config failed: %s", esp_err_to_name(err));
                    return;
                }
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
