#ifndef USR_DATA_H
#define USR_DATA_H

/**
 * @file usr_data.h
 * @brief Работа с пользовательскими данными eFuse
 */

#include <string>

namespace esp32_c3_utils
{
    /// Размер блока пользовательских данных (256 бит/32 байта)
    constexpr size_t EFUSE_USER_DATA_SIZE = 32;

    /**
     * @brief Прочитать данные из BLOCK_USR_DATA
     * @return std::string Прочитанные данные (макс. 32 байта)
     */
    std::string readUserData() noexcept;

    /**
     * @brief Записать данные в BLOCK_USR_DATA
     * @param data Данные для записи (не более 32 байт)
     * @return true если запись успешна
     * @note Записывать можно только один раз!
     */
    bool writeUserData(const std::string& data) noexcept;

    /**
     * @brief Проверить, были ли данные записаны ранее
     * @return true если данные уже записаны (изменить нельзя)
     */
    bool isUserDataWritten() noexcept;
} // namespace esp32_c3_utils

#endif //USR_DATA_H
