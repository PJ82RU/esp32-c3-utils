#include <esp32_c3_utils/crypto_utils.h>
#include <esp_log.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Пример SHA-256
    const char* message = "Hello ESP32-C3!";
    auto hash = esp32_c3_utils::computeSHA256(
        reinterpret_cast<const uint8_t*>(message),
        strlen(message)
    );

    log_i("SHA256 hash:");
    for (auto b : hash) {
        Serial.printf("%02x", b);
    }
    Serial.println();

    // Пример AES-256
    uint8_t key[32] = {0}; // Замените реальным ключом
    uint8_t iv[16] = {0};  // Замените реальным IV
    uint8_t data[32] = "Secret message to encrypt!";

    if (esp32_c3_utils::aes256Encrypt(key, iv, data, sizeof(data))) {
        log_i("Encrypted data:");
        for (size_t i = 0; i < sizeof(data); i++) {
            Serial.printf("%02x", data[i]);
        }
        Serial.println();
    }
}

void loop() {}