#include "esp32_c3_utils/temp_sensor.h"
#include <esp_log.h>

namespace
{
    constexpr auto TAG = "TempSensor";
    bool g_tempSensorInitialized = false;
}

namespace esp32_c3::utils
{
    bool isTempSensorInitialized() noexcept
    {
        return g_tempSensorInitialized;
    }

    esp_err_t initTempSensor(const temp_sensor_dac_offset_t dacOffset) noexcept
    {
        if (g_tempSensorInitialized)
        {
            ESP_LOGW(TAG, "Sensor already initialized");
            return ESP_OK;
        }

        const temp_sensor_config_t tempSensorConfig = {
            .dac_offset = dacOffset,
            .clk_div = 6
        };

        esp_err_t err = temp_sensor_set_config(tempSensorConfig);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Config failed: 0x%X", err);
            return err;
        }

        err = temp_sensor_start();
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Start failed: 0x%X", err);
            return err;
        }

        g_tempSensorInitialized = true;
        ESP_LOGI(TAG, "Initialized with dacOffset=%d", dacOffset);
        return ESP_OK;
    }

    esp_err_t getChipTemperature(float& temperature) noexcept
    {
        if (!g_tempSensorInitialized)
        {
            ESP_LOGE(TAG, "Sensor not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        return temp_sensor_read_celsius(&temperature);
    }

    void deinitTempSensor() noexcept
    {
        if (!g_tempSensorInitialized)
        {
            return;
        }

        temp_sensor_stop();
        g_tempSensorInitialized = false;
        ESP_LOGI(TAG, "Deinitialized");
    }
} // namespace esp32_c3::utils
