#include "esp32_c3_utils/clock_utils.h"
#include <cstring>
#include <string>

namespace esp32_c3::utils
{
    std::string formatTime(unsigned long timeMs,
                           const bool showDay,
                           const bool showHour,
                           const bool showMinute,
                           const bool showSecond,
                           const char daySep,
                           const char timeSep)
    {
        timeMs /= 1000; // секунды
        const uint8_t seconds = timeMs % 60;
        timeMs /= 60; // минуты
        const uint8_t minutes = timeMs % 60;
        timeMs /= 60; // часы
        const uint8_t hours = timeMs % 24;
        const uint16_t days = timeMs / 24;

        std::string result;
        result.reserve(16); // Оптимизация: "DD.HH:MM:SS" = максимум 11 символов

        // Дни (если включены и > 0)
        if (showDay && days > 0)
        {
            if (days >= 100) result += '0' + (days / 100) % 10; // Поддержка >99 дней
            if (days >= 10) result += '0' + (days / 10) % 10;
            result += '0' + days % 10;
            if (showHour || showMinute || showSecond) result += daySep;
        }

        // Часы (если включены)
        if (showHour)
        {
            result += '0' + hours / 10;
            result += '0' + hours % 10;
            if (showMinute || showSecond) result += timeSep;
        }

        // Минуты (если включены)
        if (showMinute)
        {
            result += '0' + minutes / 10;
            result += '0' + minutes % 10;
            if (showSecond) result += timeSep;
        }

        // Секунды (если включены)
        if (showSecond)
        {
            result += '0' + seconds / 10;
            result += '0' + seconds % 10;
        }

        return result;
    }
} // namespace esp32_c3::utils
