#ifndef CORE_DUMP_H
#define CORE_DUMP_H

/**
 * @file core_dump.h
 * @brief Утилиты для работы с core dump на ESP32-C3
 */

namespace esp32_c3_utils {

    /**
     * @brief Проверить и восстановить core dump
     * @details Проверяет целостность core dump, при необходимости
     *          удаляет поврежденный и создает новый
     * @note Требует включенной опции CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH
     */
    void checkAndFixCoreDump() noexcept;

} // namespace esp32_c3_utils

#endif //CORE_DUMP_H
