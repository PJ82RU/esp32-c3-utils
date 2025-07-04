#ifndef ESP32_C3_UTILS_LED_H
#define ESP32_C3_UTILS_LED_H

#include <Arduino.h>

namespace esp32_c3_objects
{
    /**
     * @brief Режимы работы светодиода
     */
    enum class LedMode
    {
        OFF,          ///< Светодиод выключен
        ON,           ///< Светодиод включен постоянно
        BLINK,        ///< Одиночное мигание
        DOUBLE_BLINK, ///< Двойное мигание
        TRIPLE_BLINK  ///< Тройное мигание
    };

    /**
     * @brief Класс для управления светодиодом
     */
    class Led
    {
    public:
        /**
         * @brief Конструктор объекта светодиода
         * @param pin Номер GPIO пина (по умолчанию GPIO_NUM_NC)
         */
        explicit Led(gpio_num_t pin = GPIO_NUM_NC) noexcept;

        /**
         * @brief Инициализация или изменение GPIO пина светодиода
         * @param pin Номер GPIO пина для управления светодиодом
         */
        void init(gpio_num_t pin) noexcept;

        /**
         * @brief Установка режима работы светодиода
         * @param mode Режим работы из перечисления LedMode
         */
        void setMode(LedMode mode) noexcept;

        /**
         * @brief Обновление состояния светодиода согласно текущему режиму
         * @param currentTime Текущее время в миллисекундах (0 - использовать millis())
         */
        void update(uint32_t currentTime = 0) noexcept;

        /// Интервал мигания в миллисекундах
        uint16_t blinkInterval = 500;

    private:
        /// Приватные методы
        void updateOutput() const noexcept;

        /// Переменные
        // Примитивные типы
        bool mIsOn = false;       ///< Флаг текущего состояния светодиода
        uint8_t mStep = 0;        ///< Текущий шаг в последовательности мигания
        uint32_t mNextUpdate = 0; ///< Время следующего обновления состояния

        // Пользовательские типы
        gpio_num_t mPin = GPIO_NUM_NC; ///< Номер GPIO пина светодиода
        LedMode mMode = LedMode::OFF;  ///< Текущий режим работы
    };
} // namespace esp32_c3_utils

#endif // ESP32_C3_UTILS_LED_H
