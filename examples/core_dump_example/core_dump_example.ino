#include <esp32_c3_utils/core_dump.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    log_d("Checking core dump...");
    esp32_c3_utils::checkAndFixCoreDump();

    log_i("System started successfully");
}

void loop() {
    // Ваш основной код
}