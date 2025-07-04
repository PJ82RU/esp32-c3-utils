# Библиотека ESP32-C3 Utils

![Лицензия](https://img.shields.io/badge/license-Unlicense-blue.svg)
![PlatformIO](https://img.shields.io/badge/platform-ESP32--C3-green.svg)

Комплексная библиотека утилит для работы с ESP32-C3, включающая:

- Управление питанием (частота CPU, режимы сна)
- Криптографические функции (SHA-256, AES-256)
- Работа с RTC-памятью
- Получение системной информации
- Управление встроенным датчиком температуры
- Работа с core dump
- Потокобезопасные объекты (очереди, семафоры, потоки)
- Управление светодиодами
- Система callback-функций

## Установка

### PlatformIO
Добавьте в ваш platformio.ini:
```ini
lib_deps =
    https://github.com/PJ82RU/esp32-c3-utils.git
```

### Arduino IDE
1. Скачайте ZIP-архив с GitHub
2. Скетч → Подключить библиотеку → Добавить .ZIP библиотеку

## Основные возможности

### Управление питанием
```cpp
#include "esp32_c3_utils/power_utils.h"

// Установка частоты CPU
esp32_c3_utils::setCpuFrequency(esp32_c3_utils::CpuFrequency::MHz_160);

// Переход в глубокий сон
esp32_c3_utils::enterPowerSave(esp32_c3_utils::PowerSaveMode::DEEP_SLEEP);
```

### Криптографические функции
```cpp
#include "esp32_c3_utils/crypto_utils.h"

// Вычисление SHA-256 хеша
auto hash = esp32_c3_utils::computeSHA256(data, size);

// Шифрование AES-256
esp32_c3_utils::aes256Encrypt(key, iv, data, size);
```

### Системная информация
```cpp
#include "esp32_c3_utils/system_info.h"

esp32_c3_utils::SystemInfo info;
if(esp32_c3_utils::getSystemInfo(info)) {
log_d("Частота CPU: %d МГц", info.cpuFreqMhz);
}
```

### Работа с RTC-памятью
```cpp
#include "esp32_c3_utils/rtc_utils.h"

// Запись данных в RTC-память
uint32_t data = 0x12345678;
esp32_c3_utils::writeRtcMemory(0, &data, sizeof(data));
```

### Управление светодиодом
```cpp
#include "esp32_c3_objects/led.h"

// Создание объекта светодиода
esp32_c3_objects::Led led(GPIO_NUM_8);
led.setMode(esp32_c3_objects::LedMode::BLINK);

// В цикле loop()
led.update();
```

## Примеры использования

Полные примеры использования доступны в папке `examples`.

## Лицензия

Данная библиотека распространяется как свободное и бесплатное программное обеспечение, переданное в общественное достояние.

Вы можете свободно копировать, изменять, публиковать, использовать, компилировать, продавать или распространять это программное обеспечение в исходном коде или в виде скомпилированного бинарного файла для любых целей, коммерческих или некоммерческих, и любыми средствами.