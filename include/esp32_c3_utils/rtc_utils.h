#ifndef RTC_UTILS_H
#define RTC_UTILS_H

/**
 * @file rtc_utils.h
 * @brief Утилиты для работы с RTC-памятью ESP32-C3
 */

#include <cstddef>

namespace esp32_c3_utils
{
    /// Размер RTC медленной памяти (8KB для ESP32-C3)
    constexpr size_t RTC_SLOW_MEM_SIZE = 8 * 1024;

    /**
     * @brief Записать данные в RTC-память
     * @param offset Смещение в RTC-памяти (байты)
     * @param data Указатель на данные
     * @param size Размер данных (не более RTC_SLOW_MEM_SIZE)
     * @return true если запись успешна
     * @note Данные сохраняются между глубокими снами
     */
    bool writeRtcMemory(size_t offset, const void* data, size_t size) noexcept;

    /**
     * @brief Прочитать данные из RTC-памяти
     * @param offset Смещение в RTC-памяти (байты)
     * @param buffer Буфер для данных
     * @param size Размер данных
     * @return true если чтение успешно
     */
    bool readRtcMemory(size_t offset, void* buffer, size_t size) noexcept;

    /**
     * @brief Очистить область RTC-памяти
     * @param offset Смещение в RTC-памяти (байты)
     * @param size Количество байт для очистки
     */
    void clearRtcMemory(size_t offset, size_t size) noexcept;
} // namespace esp32_c3_utils

#endif //RTC_UTILS_H
