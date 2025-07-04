#include <esp32_c3_utils/system_info.h>
#include <esp32_c3_utils/temp_sensor.h>

void setup() {
    Serial.begin(115200);
    esp32_c3_utils::initTempSensor();

    esp32_c3_utils::SystemInfo info;
    if (esp32_c3_utils::getSystemInfo(info)) {
        Serial.println("=== System Info ===");
        Serial.printf("Chip: %s\n", info.chipModel);
        Serial.printf("Cores: %u\n", info.chipCores);
        Serial.printf("CPU Freq: %u MHz\n", info.cpuFreqMhz);
        Serial.printf("Temperature: %.1f C\n", info.chipTemperature);
        Serial.printf("Free Heap: %u bytes\n", info.freeHeap);
    } else {
        Serial.println("Failed to get system info!");
    }
}

void loop() {}