#ifndef ESP32_C3_BYTES_UTILS_H
#define ESP32_C3_BYTES_UTILS_H

#include <Arduino.h>

namespace esp32_c3_utils
{
    /**
     * @brief Конвертирует массив байт в HEX-строку
     * @param bytes Указатель на массив байт
     * @param size Размер массива
     * @param upperCase Флаг использования верхнего регистра
     * @return HEX-строка
     */
    String bytesToHex(const uint8_t* bytes, size_t size, bool upperCase = true) noexcept;

    /**
     * @brief Конвертирует HEX-строку в массив байт
     * @param hex HEX-строка
     * @param bytes Указатель на массив для результата
     * @param size Размер массива
     * @return true в случае успеха
     */
    bool hexToBytes(const String& hex, uint8_t* bytes, size_t size) noexcept;

    /**
     * @brief Сравнивает два массива байт
     * @param buf1 Первый массив
     * @param buf2 Второй массив
     * @param size Размер массивов
     * @return true если массивы идентичны
     */
    bool compareBytes(const uint8_t* buf1, const uint8_t* buf2, size_t size) noexcept;

    /**
     * @brief Меняет местами байты в 16-битном значении
     * @param value Исходное значение
     * @return Значение с переставленными байтами
     */
    constexpr uint16_t swapBytes(uint16_t value) noexcept;
} // namespace esp32_c3_utils

#endif // ESP32_C3_BYTES_UTILS_H
