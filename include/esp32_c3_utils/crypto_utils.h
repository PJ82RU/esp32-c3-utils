#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

/**
 * @file crypto_utils.h
 * @brief Криптографические утилиты для ESP32-C3
 */

#include <array>

namespace esp32_c3_utils
{
    /// Размер SHA-256 хеша в байтах
    constexpr size_t SHA256_SIZE = 32;

    /// Размер блока AES в байтах
    constexpr size_t AES_BLOCK_SIZE = 16;

    /// Размер ключа AES-256 в байтах
    constexpr size_t AES256_KEY_SIZE = 32;

    /**
     * @brief Вычислить SHA-256 хеш
     * @param data Указатель на входные данные
     * @param size Размер данных в байтах
     * @return std::array<uint8_t, SHA256_SIZE> Хеш-сумма
     */
    std::array<uint8_t, SHA256_SIZE> computeSHA256(const uint8_t* data, size_t size) noexcept;

    /**
     * @brief Шифрование AES-256 (CBC режим)
     * @param key Ключ шифрования (32 байта)
     * @param iv Вектор инициализации (16 байт)
     * @param data Данные для шифрования (должны быть кратны AES_BLOCK_SIZE)
     * @param size Размер данных в байтах
     * @return true если шифрование успешно
     * @note Размер данных должен быть кратен 16 байтам
     */
    bool aes256Encrypt(const uint8_t (&key)[AES256_KEY_SIZE],
                       const uint8_t (&iv)[AES_BLOCK_SIZE],
                       uint8_t* data,
                       size_t size) noexcept;

    /**
     * @brief Дешифрование AES-256 (CBC режим)
     * @param key Ключ шифрования (32 байта)
     * @param iv Вектор инициализации (16 байт)
     * @param data Данные для дешифрования
     * @param size Размер данных в байтах
     * @return true если дешифрование успешно
     */
    bool aes256Decrypt(const uint8_t (&key)[AES256_KEY_SIZE],
                       const uint8_t (&iv)[AES_BLOCK_SIZE],
                       uint8_t* data,
                       size_t size) noexcept;
} // namespace esp32_c3_utils

#endif //CRYPTO_UTILS_H
