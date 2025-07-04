#include <esp32_c3_utils/usr_data.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Проверяем, записаны ли данные ранее
    if (!esp32_c3_utils::isUserDataWritten()) {
        Serial.println("Writing data to eFuse...");
        if (esp32_c3_utils::writeUserData("MyConfig123")) {
            Serial.println("Write successful!");
        } else {
            Serial.println("Write failed!");
        }
    }

    // Чтение данных
    auto data = esp32_c3_utils::readUserData();
    Serial.printf("User data: %s\n", data.c_str());
}

void loop() {}