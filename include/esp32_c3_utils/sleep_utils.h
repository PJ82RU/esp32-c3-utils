#ifndef SLEEP_UTILS_H
#define SLEEP_UTILS_H

/**
 * @file sleep_utils.h
 * @brief Утилиты для управления режимами сна ESP32-C3
 */

#include <driver/gpio.h>

namespace esp32_c3_utils
{
    /**
     * @brief Режимы пробуждения
     */
    enum class WakeupSource
    {
        TIMER,    ///< Таймер
        GPIO,     ///< GPIO (EXT0/EXT1)
        TOUCHPAD, ///< Сенсорные контакты
        ULP       ///< ULP сопроцессор
    };

    /**
     * @brief Конфигурация пробуждения по GPIO
     */
    struct GpioWakeupConfig
    {
        gpio_num_t pin; ///< Номер GPIO
        bool activeLow; ///< Активный уровень
    };

    /**
     * @brief Перевести устройство в глубокий сон
     * @param sleepDurationMs Длительность сна (0 - бесконечно)
     * @param wakeupSrc Источник пробуждения
     * @param gpioCfg Конфигурация GPIO (если выбран wakeup_src = GPIO)
     */
    void deepSleep(uint64_t sleepDurationMs = 0,
                   WakeupSource wakeupSrc = WakeupSource::TIMER,
                   GpioWakeupConfig gpioCfg = {GPIO_NUM_NC, false}) noexcept;

    /**
     * @brief Получить причину пробуждения
     * @return WakeupSource
     */
    WakeupSource getWakeupCause() noexcept;
} // namespace esp32_c3_utils

#endif //SLEEP_UTILS_H
