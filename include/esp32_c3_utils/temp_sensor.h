#ifndef ESP32_C3_UTILS_TEMP_SENSOR_H
#define ESP32_C3_UTILS_TEMP_SENSOR_H

/**
 * @file temp_sensor.h
 * @brief Утилиты для работы со встроенным датчиком температуры ESP32-C3
 */

#include <driver/temp_sensor.h>

namespace esp32_c3::utils
{
    /**
     * @brief Проверить состояние инициализации датчика температуры
     * @return true если датчик инициализирован и готов к работе
     */
    bool isTempSensorInitialized() noexcept;

    /**
     * @brief Инициализировать внутренний датчик температуры
     * @param dacOffset Смещение DAC (TSENS_DAC_L0 = -10, L4 = +10)
     * @return esp_err_t Код ошибки, ESP_OK при успешной инициализации
     */
    esp_err_t initTempSensor(temp_sensor_dac_offset_t dacOffset = TSENS_DAC_DEFAULT) noexcept;

    /**
     * @brief Получить текущую температуру чипа
     * @param[out] temperature Ссылка для сохранения значения температуры
     * @return esp_err_t Код ошибки, ESP_OK при успешном получении температуры
     */
    esp_err_t getChipTemperature(float& temperature) noexcept;

    /**
     * @brief Деинициализировать датчик температуры
     */
    void deinitTempSensor() noexcept;
} // namespace esp32_c3::utils

#endif // ESP32_C3_UTILS_TEMP_SENSOR_H
