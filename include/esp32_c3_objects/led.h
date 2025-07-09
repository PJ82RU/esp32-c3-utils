#ifndef ESP32_C3_UTILS_LED_H
#define ESP32_C3_UTILS_LED_H

#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>

namespace esp32_c3::objects
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
     * @brief Класс для управления светодиодом с активным низким уровнем
     *
     * @details Поддерживает различные режимы работы светодиода:
     * - Постоянное включение/выключение
     * - Различные варианты мигания
     * - Автоматическое обновление состояния
     * - Использует системный таймер ESP-IDF для временных интервалов
     */
    class Led
    {
    public:
        static constexpr auto TAG = "LED"; ///< Тег для логирования

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
         * @param currentTime Текущее время в микросекундах (0 - использовать esp_timer_get_time())
         */
        void update(uint64_t currentTime = esp_timer_get_time()) noexcept;

        uint16_t blinkInterval = 500; ///< Интервал мигания в миллисекундах

    private:
        /**
         * @brief Обновление физического состояния вывода GPIO
         */
        void updateOutput() const noexcept;

        /**
         * @brief Конвертирует миллисекунды в микросекунды
         * @param ms Время в миллисекундах
         * @return Время в микросекундах
         */
        static constexpr uint64_t msToUs(const uint32_t ms) noexcept
        {
            return static_cast<uint64_t>(ms) * 1000;
        }

        // Примитивные типы
        bool mIsOn = false;       ///< Флаг текущего состояния светодиода
        uint8_t mStep = 0;        ///< Текущий шаг в последовательности мигания
        uint64_t mNextUpdate = 0; ///< Время следующего обновления состояния (в микросекундах)

        // Пользовательские типы
        gpio_num_t mPin = GPIO_NUM_NC; ///< Номер GPIO пина светодиода
        LedMode mMode = LedMode::OFF;  ///< Текущий режим работы
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_LED_H
