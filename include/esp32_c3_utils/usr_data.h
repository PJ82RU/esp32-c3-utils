#ifndef ESP32_C3_UTILS_USR_DATA_H
#define ESP32_C3_UTILS_USR_DATA_H

/**
 * @file usr_data.h
 * @brief Работа с пользовательскими данными в eFuse (BLOCK_USR_DATA)
 */

#include <string>

namespace esp32_c3::utils
{
    /// Максимальный размер пользовательских данных в eFuse (32 байта)
    constexpr size_t EFUSE_USER_DATA_SIZE = 32;

    /**
     * @brief Прочитать данные из BLOCK_USR_DATA
     * @return std::string Прочитанные данные (максимум 32 байта)
     * @note Возвращаемая строка может быть короче, если данные не заполняют весь блок
     */
    std::string readUserData() noexcept;

    /**
     * @brief Записать данные в BLOCK_USR_DATA
     * @param data Данные для записи (не более 32 байт)
     * @return true если запись успешна
     * @note Данные можно записать только один раз!
     */
    bool writeUserData(const std::string& data) noexcept;

    /**
     * @brief Проверить состояние записи пользовательских данных
     * @return true если данные уже были записаны (изменить нельзя)
     */
    bool isUserDataWritten() noexcept;
} // namespace esp32_c3::utils

#endif // ESP32_C3_UTILS_USR_DATA_H
