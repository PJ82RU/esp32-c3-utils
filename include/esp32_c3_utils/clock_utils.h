#ifndef ESP32_C3_CLOCK_UTILS_H
#define ESP32_C3_CLOCK_UTILS_H

namespace esp32_c3::utils
{
    /**
     * @brief Форматирует время в читаемую строку
     * @param buffer Буфер для результата (минимум 16 байт)
     * @param time Время в миллисекундах
     * @param showDay Флаг отображения дней
     * @param showHour Флаг отображения часов
     * @param showMinute Флаг отображения минут
     * @param showSecond Флаг отображения секунд
     */
    void formatTime(char buffer[16],
                    unsigned long time,
                    bool showDay = false,
                    bool showHour = true,
                    bool showMinute = true,
                    bool showSecond = true) noexcept;
} // namespace esp32_c3::utils

#endif // ESP32_C3_CLOCK_UTILS_H
