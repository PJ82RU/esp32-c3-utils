#include <esp32_c3_utils/rtc_utils.h>

// Область для счетчика в RTC-памяти
constexpr size_t COUNTER_OFFSET = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Чтение счетчика из RTC-памяти
    uint32_t counter = 0;
    if (esp32_c3_utils::readRtcMemory(COUNTER_OFFSET, &counter, sizeof(counter))) {
        Serial.printf("Previous counter: %u\n", counter);
        counter++;
    } else {
        Serial.println("No valid counter found, initializing to 1");
        counter = 1;
    }

    // Запись обновленного счетчика
    if (!esp32_c3_utils::writeRtcMemory(COUNTER_OFFSET, &counter, sizeof(counter))) {
        Serial.println("Failed to write counter to RTC!");
    }

    // Проверка записи
    uint32_t verify_counter = 0;
    if (esp32_c3_utils::readRtcMemory(COUNTER_OFFSET, &verify_counter, sizeof(verify_counter))) {
        Serial.printf("Current counter: %u\n", verify_counter);
    }
}

void loop() {
    delay(1000);
}