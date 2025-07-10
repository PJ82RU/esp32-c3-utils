#ifndef ESP32_C3_UTILS_H
#define ESP32_C3_UTILS_H

/**
 * @file esp32_c3_utils.h
 * @brief Главный заголовочный файл библиотеки ESP32-C3 Utils
 */

/// Утилиты
#include "esp32_c3_utils/bytes_utils.h"
#include "esp32_c3_utils/clock_utils.h"
#include "esp32_c3_utils/crypto_utils.h"
#include "esp32_c3_utils/power_utils.h"
#include "esp32_c3_utils/rtc_utils.h"
#include "esp32_c3_utils/sleep_utils.h"
#include "esp32_c3_utils/system_info.h"
#include "esp32_c3_utils/core_dump.h"
#include "esp32_c3_utils/usr_data.h"
/// Объекты
#include "esp32_c3_objects/callback.h"
#include "esp32_c3_objects/led.h"
#include "esp32_c3_objects/queue.h"
#include "esp32_c3_objects/simple_callback.h"
#include "esp32_c3_objects/thread.h"
#include "esp32_c3_objects/temp_sensor.h"

#endif //ESP32_C3_UTILS_H
