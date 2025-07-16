#include "esp32_c3_objects/temp_sensor.h"
#include <esp_log.h>
#include <esp_check.h>

namespace esp32_c3::objects
{
    TempSensor::TempSensor(const int rangeMin, const int rangeMax) noexcept
    {
        const temperature_sensor_config_t config = {
            .range_min = rangeMin,
            .range_max = rangeMax,
            .clk_src = TEMPERATURE_SENSOR_CLK_SRC_DEFAULT,
            .flags = {
                .allow_pd = false
            }
        };

        esp_err_t ret = temperature_sensor_install(&config, &mHandle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Initialization failed: %s", esp_err_to_name(ret));
            return;
        }

        ret = temperature_sensor_enable(mHandle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Activation failed: %s", esp_err_to_name(ret));
            temperature_sensor_uninstall(mHandle);
            mHandle = nullptr;
            return;
        }

        ESP_LOGI(TAG, "Sensor initialized (range: %d..%d°C)", rangeMin, rangeMax);
    }

    TempSensor::~TempSensor() noexcept
    {
        if (!mHandle) return;

        // Отключаем датчик
        if (const esp_err_t err_disable = temperature_sensor_disable(mHandle);
            err_disable != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to disable sensor: %s", esp_err_to_name(err_disable));
        }

        // Удаляем драйвер
        if (const esp_err_t err_uninstall = temperature_sensor_uninstall(mHandle);
            err_uninstall != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to uninstall sensor: %s", esp_err_to_name(err_uninstall));
        }

        ESP_LOGI(TAG, "Sensor deinitialized");
        mHandle = nullptr;
    }

    bool TempSensor::isInitialized() const noexcept
    {
        return mHandle != nullptr;
    }

    std::optional<float> TempSensor::read() const noexcept
    {
        if (!mHandle) return std::nullopt;

        float temp = 0.0f;
        if (const esp_err_t ret = temperature_sensor_get_celsius(mHandle, &temp); ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Read failed: %s", esp_err_to_name(ret));
            return std::nullopt;
        }

        return temp;
    }
} // namespace esp32_c3::objects
