#include "esp32_c3_utils/crypto_utils.h"
#include <mbedtls/sha256.h>
#include <mbedtls/aes.h>
#include <esp32-hal.h>

namespace esp32_c3_utils
{
    std::array<uint8_t, SHA256_SIZE> computeSHA256(const uint8_t* data, const size_t size) noexcept
    {
        std::array<uint8_t, SHA256_SIZE> hash{};

        if (!data || size == 0)
        {
            log_e("Invalid SHA256 input");
            return hash;
        }

        mbedtls_sha256_context ctx;
        mbedtls_sha256_init(&ctx);

        mbedtls_sha256_starts(&ctx, 0); // 0 = SHA-256
        mbedtls_sha256_update(&ctx, data, size);
        mbedtls_sha256_finish(&ctx, hash.data());

        mbedtls_sha256_free(&ctx);
        log_d("Computed SHA256 for %zu bytes", size);
        return hash;
    }

    bool aes256Encrypt(const uint8_t (&key)[AES256_KEY_SIZE],
                       const uint8_t (&iv)[AES_BLOCK_SIZE],
                       uint8_t* data,
                       const size_t size) noexcept
    {
        if (!data || size == 0 || size % AES_BLOCK_SIZE != 0)
        {
            log_e("Invalid AES256 input");
            return false;
        }

        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);

        bool success = false;
        uint8_t iv_copy[AES_BLOCK_SIZE];
        memcpy(iv_copy, iv, AES_BLOCK_SIZE);

        if (mbedtls_aes_setkey_enc(&ctx, key, AES256_KEY_SIZE * 8) == 0 &&
            mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, size,
                                  iv_copy, data, data) == 0)
        {
            success = true;
            log_d("AES256 encrypted %zu bytes", size);
        }
        else
        {
            log_e("AES256 encryption failed");
        }

        mbedtls_aes_free(&ctx);
        return success;
    }

    bool aes256Decrypt(const uint8_t (&key)[AES256_KEY_SIZE],
                       const uint8_t (&iv)[AES_BLOCK_SIZE],
                       uint8_t* data,
                       const size_t size) noexcept
    {
        if (!data || size == 0 || size % AES_BLOCK_SIZE != 0)
        {
            log_e("Invalid AES256 input");
            return false;
        }

        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);

        bool success = false;
        uint8_t ivCopy[AES_BLOCK_SIZE];
        memcpy(ivCopy, iv, AES_BLOCK_SIZE);

        if (mbedtls_aes_setkey_dec(&ctx, key, AES256_KEY_SIZE * 8) == 0 &&
            mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, size,
                                  ivCopy, data, data) == 0)
        {
            success = true;
            log_d("AES256 decrypted %zu bytes", size);
        }
        else
        {
            log_e("AES256 decryption failed");
        }

        mbedtls_aes_free(&ctx);
        return success;
    }
} // namespace esp32_c3_utils
