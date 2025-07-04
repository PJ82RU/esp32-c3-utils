#include <esp32_c3_utils/power_utils.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Установка частоты CPU
    esp32_c3_utils::setCpuFrequency(esp32_c3_utils::CpuFrequency::MHz_160);

    // Чтение напряжения батареи
    float voltage = esp32_c3_utils::readBatteryVoltage();
    Serial.printf("Battery voltage: %.2fV\n", voltage);

    // Переход в режим энергосбережения
    if (voltage < 3.3f) {
        Serial.println("Entering power save mode...");
        esp32_c3_utils::enterPowerSave(
            esp32_c3_utils::PowerSaveMode::LIGHT_SLEEP,
            10000 // 10 секунд
        );
    }
}

void loop() {
    // Основной код
}