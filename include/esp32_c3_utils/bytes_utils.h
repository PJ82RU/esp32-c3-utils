#ifndef ESP32_C3_BYTES_UTILS_H
#define ESP32_C3_BYTES_UTILS_H

#include <cstdint>
#include <string>

namespace esp32_c3::utils
{
    /**
     * @brief Конвертирует массив байт в HEX-строку
     * @param bytes Указатель на массив байт
     * @param size Размер массива
     * @param upperCase Флаг использования верхнего регистра
     * @return HEX-строка
     */
    std::string bytesToHex(const uint8_t* bytes, size_t size, bool upperCase = true) noexcept;

    /**
     * @brief Конвертирует HEX-строку в массив байт
     * @param hex HEX-строка
     * @param bytes Указатель на массив для результата
     * @param size Размер массива
     * @return true в случае успеха
     */
    bool hexToBytes(const std::string& hex, uint8_t* bytes, size_t size) noexcept;

} // namespace esp32_c3::utils

#endif // ESP32_C3_BYTES_UTILS_H
