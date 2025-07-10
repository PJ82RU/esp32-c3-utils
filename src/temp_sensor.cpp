#include "esp32_c3_objects/temp_sensor.h"
#include <esp_log.h>
#include <esp_check.h>

namespace esp32_c3::objects
{
    std::unique_ptr<TempSensor> TempSensor::create(const int rangeMin, const int rangeMax) noexcept
    {
        temperature_sensor_handle_t handle = nullptr;
        const temperature_sensor_config_t config = {
            .range_min = rangeMin,
            .range_max = rangeMax,
            .clk_src = TEMPERATURE_SENSOR_CLK_SRC_DEFAULT,
            .flags = {
                .allow_pd = false
            }
        };

        esp_err_t ret = temperature_sensor_install(&config, &handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Initialization failed: %s", esp_err_to_name(ret));
            return nullptr;
        }

        ret = temperature_sensor_enable(handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Activation failed: %s", esp_err_to_name(ret));
            temperature_sensor_uninstall(handle);
            return nullptr;
        }

        ESP_LOGI(TAG, "Sensor initialized (range: %d..%d°C)", rangeMin, rangeMax);
        return std::unique_ptr<TempSensor>(new TempSensor(handle));
    }

    TempSensor::TempSensor(temperature_sensor_handle_t handle) noexcept
        : mHandle(handle)
    {
    }

    TempSensor::~TempSensor() noexcept
    {
        if (mHandle)
        {
            ESP_ERROR_CHECK(temperature_sensor_disable(mHandle));
            ESP_ERROR_CHECK(temperature_sensor_uninstall(mHandle));
            ESP_LOGI(TAG, "Sensor deinitialized");
        }
    }

    std::optional<float> TempSensor::read() const noexcept
    {
        float temp = 0.0f;

        if (const esp_err_t ret = temperature_sensor_get_celsius(mHandle, &temp); ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Read failed: %s", esp_err_to_name(ret));
            return std::nullopt;
        }

        return temp;
    }
} // namespace esp32_c3::objects
