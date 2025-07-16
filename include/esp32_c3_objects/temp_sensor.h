#ifndef ESP32_C3_UTILS_TEMP_SENSOR_H
#define ESP32_C3_UTILS_TEMP_SENSOR_H

/**
 * @file temp_sensor.h
 * @brief Обертка для работы с температурным датчиком ESP32-C3 (ESP-IDF v5.x)
 */

#include <driver/temperature_sensor.h>
#include <memory>
#include <optional>

namespace esp32_c3::objects
{
    /**
     * @brief Класс для работы с внутренним датчиком температуры чипа
     * @details Использует новый драйвер temperature_sensor из ESP-IDF v5+
     */
    class TempSensor
    {
    public:
        /// @brief Тег для логирования
        static constexpr auto TAG = "TempSensor";

        /**
         * @brief Конструктор датчика температуры
         * @param rangeMin Минимальный диапазон измерения (°C)
         * @param rangeMax Максимальный диапазон измерения (°C)
         */
        explicit TempSensor(
            int rangeMin = -10,
            int rangeMax = 80) noexcept;

        ~TempSensor() noexcept;

        /**
         * @brief Проверить, успешно ли инициализирован датчик
         * @return true если датчик готов к использованию
         */
        [[nodiscard]] bool isInitialized() const noexcept;

        /**
         * @brief Получить текущую температуру чипа
         * @return Температура в °C или std::nullopt при ошибке
         */
        [[nodiscard]] std::optional<float> read() const noexcept;

        // Удаляем конструкторы копирования и присваивания
        TempSensor(const TempSensor&) = delete;
        TempSensor& operator=(const TempSensor&) = delete;

    private:
        temperature_sensor_handle_t mHandle = nullptr; ///< Хэндл драйвера датчика
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_TEMP_SENSOR_H
