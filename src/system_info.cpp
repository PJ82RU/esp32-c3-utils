#include "esp32_c3_utils/system_info.h"
#include "esp32_c3_utils/temp_sensor.h"
#include <Arduino.h>
#include <esp_log.h>
#include <esp_mac.h>

namespace esp32_c3_utils
{
    bool getSystemInfo(SystemInfo& info) noexcept
    {
        // Обнуление структуры
        info = {};

        // Получение температуры
        const bool tempWasInitialized = isTempSensorInitialized();
        if (tempWasInitialized || initTempSensor() == ESP_OK)
        {
            float temp = 0.0f;
            if (getChipTemperature(temp) == ESP_OK)
            {
                info.chipTemperature = static_cast<int16_t>(temp * 100);
            }
        }
        if (!tempWasInitialized)
        {
            deinitTempSensor();
        }

        // Заполнение остальных полей
        esp_efuse_mac_get_default(info.macArray);
        info.cpuFreqMhz = ESP.getCpuFreqMHz();
        info.cycleCount = ESP.getCycleCount();
        info.flashChipSize = ESP.getFlashChipSize();
        info.flashChipSpeed = ESP.getFlashChipSpeed();
        info.freeHeap = ESP.getFreeHeap();
        info.heapSize = ESP.getHeapSize();
        info.sketchSize = ESP.getSketchSize();
        info.chipCores = ESP.getChipCores();
        info.chipRevision = ESP.getChipRevision();

        strncpy(info.chipModel, ESP.getChipModel(), sizeof(info.chipModel));
        strncpy(info.sdkVersion, ESP.getSdkVersion(), sizeof(info.sdkVersion));
        strncpy(info.sketchMd5, ESP.getSketchMD5().c_str(), sizeof(info.sketchMd5));

        ESP_LOGI("SYSINFO", "System info collected. Temp: %.2f°C",
                 info.chipTemperature / 100.0f);
        return true;
    }
} // namespace esp32_c3_utils
