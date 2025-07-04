#ifndef ESP32_C3_UTILS_SYSTEM_INFO_H
#define ESP32_C3_UTILS_SYSTEM_INFO_H

/**
 * @file system_info.h
 * @brief Системная информация ESP32-C3
 */

#include <cstdint>
#include <cstring>

namespace esp32_c3_utils
{
    constexpr size_t CHIP_MODEL_LEN = 32; ///< Макс. длина модели чипа
    constexpr size_t SDK_VERSION_LEN = 8; ///< Макс. версия SDK
    constexpr size_t SKETCH_MD5_LEN = 16; ///< Длина MD5 (16 байт)

    /**
     * @brief Структура системной информации
     * @note Оптимизированное выравнивание и сортировка полей
     */
#pragma pack(push, 1)
    struct SystemInfo
    {
        // 64-битные поля (выравнивание 8)
        union
        {
            uint64_t efuseMac;   ///< MAC как uint64_t
            uint8_t macArray[6]; ///< MAC как массив байт + 2 байта padding
        };

        // 32-битные поля (выравнивание 4)
        uint32_t cpuFreqMhz;     ///< Частота CPU (МГц)
        uint32_t cycleCount;     ///< Счетчик циклов
        uint32_t flashChipSize;  ///< Размер flash (байт)
        uint32_t flashChipSpeed; ///< Частота flash (МГц)
        uint32_t freeHeap;       ///< Свободная heap-память (байт)
        uint32_t heapSize;       ///< Общий размер heap (байт)
        uint32_t sketchSize;     ///< Размер скетча (байт)

        // 16-битные поля (выравнивание 2)
        int16_t chipTemperature; ///< Температура (°C * 100)

        // 8-битные поля
        uint8_t chipCores;    ///< Количество ядер (1 или 2)
        uint8_t chipRevision; ///< Ревизия чипа

        // Строковые поля (выравнивание 1)
        char chipModel[CHIP_MODEL_LEN];   ///< Модель чипа
        char sdkVersion[SDK_VERSION_LEN]; ///< Версия SDK
        char sketchMd5[SKETCH_MD5_LEN];   ///< MD5 скетча
    };
#pragma pack(pop)

    /**
     * @brief Заполнить структуру системной информацией
     * @param[out] info Ссылка на структуру
     * @return true в случае успеха
     */
    bool getSystemInfo(SystemInfo& info) noexcept;
} // namespace esp32_c3_utils

#endif //ESP32_C3_UTILS_SYSTEM_INFO_H
