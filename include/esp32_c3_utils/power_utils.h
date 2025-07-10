#ifndef ESP32_C3_POWER_UTILS_H
#define ESP32_C3_POWER_UTILS_H

/**
 * @file power_utils.h
 * @brief Утилиты для управления питанием ESP32-C3
 */

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

namespace esp32_c3::utils
{
    /**
     * @brief Доступные частоты CPU
     */
    enum class CpuFrequency
    {
        MHz_80  = 80,  ///< Энергоэффективный режим
        MHz_160 = 160, ///< Максимальная производительность
        MIN     = MHz_80,
        MAX     = MHz_160
    };

    /**
     * @brief Установить частоту CPU
     * @param freq Частота из enum CpuFrequency
     * @return true если успешно, false при ошибке
     */
    bool setCpuFrequency(CpuFrequency freq) noexcept;

    /**
     * @brief Получить текущее напряжение батареи
     * @param adcPin Аналоговый пин (по умолчанию GPIO0)
     * @param voltageDivider Коэффициент делителя напряжения (по умолчанию 1.0)
     * @return Напряжение в вольтах
     */
    float readBatteryVoltage(gpio_num_t adcPin = GPIO_NUM_0,
                             float voltageDivider = 1.0f) noexcept;

    /**
     * @brief Режим энергосбережения
     */
    enum class PowerSaveMode
    {
        LIGHT_SLEEP, ///< Легкий сон с пробуждением по таймеру
        DEEP_SLEEP,  ///< Глубокий сон с сохранением RTC памяти
        OFF          ///< Полное выключение (требуется аппаратный сброс)
    };

    /**
     * @brief Активировать режим энергосбережения
     * @param mode Режим из enum PowerSaveMode
     * @param durationMs Длительность сна в мс (для LIGHT_SLEEP)
     */
    void enterPowerSave(PowerSaveMode mode, uint32_t durationMs = 0) noexcept;
} // namespace esp32_c3::utils

#endif //ESP32_C3_POWER_UTILS_H
