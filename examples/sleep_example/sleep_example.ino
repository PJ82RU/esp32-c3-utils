#include <esp32_c3_utils/sleep_utils.h>

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Определяем причину пробуждения
    auto cause = esp32_c3_utils::getWakeupCause();
    Serial.printf("Wakeup cause: %d\n", static_cast<int>(cause));

    // Конфигурация сна
    esp32_c3_utils::GpioWakeupConfig gpio_cfg {
        .pin = GPIO_NUM_3,
        .active_low = false
    };

    // Переход в глубокий сон на 10 сек или по GPIO3
    esp32_c3_utils::deepSleep(10000,
                             esp32_c3_utils::WakeupSource::GPIO,
                             gpio_cfg);
}

void loop() {}