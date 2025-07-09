#ifndef ESP32_C3_UTILS_CORE_DUMP_H
#define ESP32_C3_UTILS_CORE_DUMP_H

/**
 * @file core_dump.h
 * @brief Утилиты для работы с core dump на ESP32-C3
 */

namespace esp32_c3::utils {

    /**
     * @brief Проверить и восстановить core dump
     * @details Проверяет целостность core dump, при необходимости
     *          удаляет поврежденный и создает новый.
     * @note Требует включенной опции CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH
     * @warning Функция может вызвать перезагрузку при создании нового дампа
     */
    void checkAndFixCoreDump() noexcept;

} // namespace esp32_c3::utils

#endif // ESP32_C3_UTILS_CORE_DUMP_H