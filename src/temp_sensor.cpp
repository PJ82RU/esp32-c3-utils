#include "esp32_c3_utils/temp_sensor.h"
#include <esp32-hal.h>

namespace
{
    bool tempSensorInitialized = false;
}

namespace esp32_c3_utils
{
    bool isTempSensorInitialized() noexcept
    {
        return tempSensorInitialized;
    }

    esp_err_t initTempSensor(const temp_sensor_dac_offset_t dacOffset) noexcept
    {
        if (tempSensorInitialized)
        {
            log_w("TEMP", "Sensor already initialized");
            return ESP_OK;
        }

        const temp_sensor_config_t tempSensor = {
            .dac_offset = dacOffset,
            .clk_div = 6
        };

        esp_err_t err = temp_sensor_set_config(tempSensor);
        if (err != ESP_OK)
        {
            log_e("TEMP", "Config failed: 0x%X", err);
            return err;
        }

        err = temp_sensor_start();
        if (err != ESP_OK)
        {
            log_e("TEMP", "Start failed: 0x%X", err);
            return err;
        }

        tempSensorInitialized = true;
        log_i("TEMP", "Initialized with dacOffset=%d", dacOffset);
        return ESP_OK;
    }

    esp_err_t getChipTemperature(float& temperature) noexcept
    {
        if (!tempSensorInitialized)
        {
            log_e("TEMP", "Sensor not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        return temp_sensor_read_celsius(&temperature);
    }

    void deinitTempSensor() noexcept
    {
        if (!tempSensorInitialized)
        {
            return;
        }

        temp_sensor_stop();
        tempSensorInitialized = false;
        log_i("TEMP", "Deinitialized");
    }
} // namespace esp32_c3_utils
