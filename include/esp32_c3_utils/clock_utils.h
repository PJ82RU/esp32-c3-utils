#ifndef ESP32_C3_CLOCK_UTILS_H
#define ESP32_C3_CLOCK_UTILS_H

#include <string>

namespace esp32_c3::utils
{
    /**
     * @brief Форматирует временной интервал в читаемую строку
     *
     * Преобразует временной интервал в миллисекундах в строковое представление
     * формата "DD.HH:MM:SS" с возможностью настройки отображаемых компонентов
     * и разделителей.
     *
     * @param timeMs Временной интервал в миллисекундах (0..2^32-1)
     * @param showDay Флаг отображения дней (true - показывать дни)
     * @param showHour Флаг отображения часов (true - показывать часы)
     * @param showMinute Флаг отображения минут (true - показывать минуты)
     * @param showSecond Флаг отображения секунд (true - показывать секунды)
     * @param daySep Разделитель между днями и часами (по умолчанию '.')
     * @param timeSep Разделитель между часами, минутами и секундами (по умолчанию ':')
     * @return std::string Отформатированная строка времени
     *
     * @note Примеры использования:
     * @code
     * formatTime(123456789)       // "34:17:36" (только часы:минуты:секунды)
     * formatTime(123456789, true)  // "1.10:17:36" (с днями)
     * formatTime(3600000, false, true, true, false) // "01:00" (только часы:минуты)
     * @endcode
     */
    std::string formatTime(unsigned long timeMs,
                           bool showDay = false,
                           bool showHour = true,
                           bool showMinute = true,
                           bool showSecond = true,
                           char daySep = '.',
                           char timeSep = ':');
} // namespace esp32_c3::utils

#endif // ESP32_C3_CLOCK_UTILS_H
