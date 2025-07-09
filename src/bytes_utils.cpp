#include "esp32_c3_utils/bytes_utils.h"

namespace esp32_c3_utils
{
    std::string bytesToHex(const uint8_t* bytes, const size_t size, const bool upperCase) noexcept
    {
        if (bytes == nullptr || size == 0)
        {
            return {};
        }

        std::string result;
        result.reserve(size * 2); // Оптимизация: предварительное выделение памяти

        const char* hexChars = upperCase ? "0123456789ABCDEF" : "0123456789abcdef";

        for (size_t i = 0; i < size; ++i)
        {
            result += hexChars[(bytes[i] >> 4) & 0x0F];
            result += hexChars[bytes[i] & 0x0F];
        }

        return result;
    }

    bool hexToBytes(const std::string& hex, uint8_t* bytes, const size_t size) noexcept
    {
        if (hex.empty() || bytes == nullptr || size == 0 || hex.length() % 2 != 0)
        {
            return false;
        }

        const size_t len = std::min(size, hex.length() / 2);

        for (size_t i = 0, j = 0; j < len; i += 2, ++j)
        {
            const uint8_t high = (hex[i] <= '9') ? (hex[i] - '0') : ((hex[i] & 0x0F) + 9);
            const uint8_t low = (hex[i + 1] <= '9') ? (hex[i + 1] - '0') : ((hex[i + 1] & 0x0F) + 9);
            bytes[j] = (high << 4) | low;
        }

        return true;
    }

    bool compareBytes(const uint8_t* buf1, const uint8_t* buf2, const size_t size) noexcept
    {
        if (buf1 == nullptr || buf2 == nullptr)
        {
            return false;
        }

        for (size_t i = 0; i < size; ++i)
        {
            if (buf1[i] != buf2[i])
            {
                return false;
            }
        }
        return true;
    }

    constexpr uint16_t swapBytes(const uint16_t value) noexcept
    {
        return (value << 8) | (value >> 8);
    }
} // namespace esp32_c3_utils
