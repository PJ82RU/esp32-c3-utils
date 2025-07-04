#include <esp32_c3_utils/temp_sensor.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Инициализация с калибровкой L2 (по умолчанию)
    if (esp32_c3_utils::initTempSensor() != ESP_OK) {
        Serial.println("Failed to init temp sensor!");
        return;
    }

    // Чтение температуры
    float temp;
    if (esp32_c3_utils::getChipTemperature(&temp) == ESP_OK) {
        Serial.printf("Chip temperature: %.2f°C\n", temp);
    }

    // Деинициализация для экономии энергии
    esp32_c3_utils::deinitTempSensor();
}

void loop() {
    delay(1000);
}