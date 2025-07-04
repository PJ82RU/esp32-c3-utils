#ifndef ESP32_C3_UTILS_TEMP_SENSOR_H
#define ESP32_C3_UTILS_TEMP_SENSOR_H

/**
 * @file temp_sensor.h
 * @brief Утилиты для встроенного датчика температуры
 */

#include <driver/temp_sensor.h>

namespace esp32_c3_utils
{
    /**
     * @brief Проверить инициализацию датчика
     * @return true если датчик инициализирован
     */
    bool isTempSensorInitialized() noexcept;

    /**
     * @brief Инициализировать внутренний датчик температуры
     * @param dacOffset Смещение DAC (TSENS_DAC_L0 = -10, L4 = +10)
     * @return esp_err_t Код ошибки ESP_OK при успехе
     */
    esp_err_t initTempSensor(temp_sensor_dac_offset_t dacOffset = TSENS_DAC_DEFAULT) noexcept;

    /**
     * @brief Получить температуру чипа
     * @param[out] temperature Ссылка для сохранения температуры
     * @return esp_err_t Код ошибки ESP_OK при успехе
     */
    esp_err_t getChipTemperature(float& temperature) noexcept;

    /**
     * @brief Остановить датчик температуры (для экономии энергии)
     */
    void deinitTempSensor() noexcept;
} // namespace esp32_c3_utils

#endif //ESP32_C3_UTILS_TEMP_SENSOR_H
