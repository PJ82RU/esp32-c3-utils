#ifndef ESP32_C3_UTILS_SYSTEM_INFO_H
#define ESP32_C3_UTILS_SYSTEM_INFO_H

/**
 * @file system_info.h
 * @brief Получение системной информации ESP32-C3
 */

#include <cstddef>
#include <cstdint>
#include <array>

namespace esp32_c3::utils
{
    ///< Максимальная длина строки модели чипа
    constexpr size_t CHIP_MODEL_LEN = 32;
    ///< Максимальная длина версии SDK
    constexpr size_t SDK_VERSION_LEN = 8;
    ///< Длина MD5-хеша приложения
    constexpr size_t SKETCH_MD5_LEN = 16;

    /**
     * @brief Структура системной информации
     * @details Оптимизирована по выравниванию и размеру
     */
#pragma pack(push, 1)
    struct SystemInfo
    {
        uint32_t cpuFreqMhz;     ///< Частота CPU в МГц
        uint32_t cycleCount;     ///< Счетчик циклов процессора
        uint32_t flashChipSize;  ///< Размер Flash-памяти в байтах
        uint32_t flashChipSpeed; ///< Частота Flash в МГц
        uint32_t freeHeap;       ///< Свободная heap-память в байтах
        uint32_t heapSize;       ///< Общий размер heap-памяти
        uint32_t sketchSize;     ///< Размер прошивки в байтах
        uint8_t chipCores;       ///< Количество ядер процессора
        uint8_t chipRevision;    ///< Ревизия чипа

        union
        {
            uint64_t efuseMac;               ///< MAC-адрес в uint64_t
            std::array<uint8_t, 6> macArray; ///< MAC-адрес в виде массива
        };

        std::array<char, CHIP_MODEL_LEN> chipModel;    ///< Модель чипа
        std::array<char, SDK_VERSION_LEN> sdkVersion;  ///< Версия SDK
        std::array<uint8_t, SKETCH_MD5_LEN> sketchMd5; ///< MD5-хеш прошивки
    };
#pragma pack(pop)

    /**
     * @brief Получить системную информацию
     * @param[out] info Ссылка на структуру для заполнения
     * @return true если данные успешно получены
     * @note Функция не бросает исключений (noexcept)
     */
    bool getSystemInfo(SystemInfo& info) noexcept;
} // namespace esp32_c3::utils
#endif // ESP32_C3_UTILS_SYSTEM_INFO_H
