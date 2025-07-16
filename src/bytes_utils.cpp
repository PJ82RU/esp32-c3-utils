#include "esp32_c3_utils/bytes_utils.h"

namespace esp32_c3::utils
{
    std::string bytesToHex(const uint8_t* bytes, const size_t size, const bool upperCase)
    {
        if (bytes == nullptr || size == 0)
        {
            return {};
        }

        // Lookup-таблицы для быстрого преобразования
        static constexpr auto kHexCharsLower = "0123456789abcdef";
        static constexpr auto kHexCharsUpper = "0123456789ABCDEF";
        const auto* hexChars = upperCase ? kHexCharsUpper : kHexCharsLower;

        std::string result;
        result.resize(size * 2); // Заранее выделяем память (быстрее, чем reserve + push_back)

        for (size_t i = 0; i < size; ++i)
        {
            const uint8_t byte = bytes[i];
            result[i * 2] = hexChars[byte >> 4];       // Старший полубайт
            result[i * 2 + 1] = hexChars[byte & 0x0F]; // Младший полубайт
        }

        return result;
    }


    bool hexToBytes(const std::string& hex, uint8_t* bytes, const size_t size) noexcept
    {
        if (hex.empty() || bytes == nullptr || size == 0 || hex.size() % 2 != 0)
        {
            return false;
        }

        // Lookup-таблица для преобразования HEX-символов в полубайты (nibbles)
        static constexpr uint8_t kHexLookup[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 0-15
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 16-31
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 32-47
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,       // 48-63 ('0'-'9')
            0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64-79 ('A'-'F')
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 80-95
            0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96-111 ('a'-'f')
            // ... остальное заполнено нулями
        };

        const size_t len = std::min(size, hex.size() / 2);

        for (size_t i = 0, j = 0; j < len; i += 2, ++j)
        {
            const char c1 = hex[i];
            const char c2 = hex[i + 1];

            // Быстрая проверка на валидность HEX через lookup-таблицу
            if ((kHexLookup[static_cast<uint8_t>(c1)] == 0 && c1 != '0') ||
                (kHexLookup[static_cast<uint8_t>(c2)] == 0 && c2 != '0'))
            {
                return false;
            }

            bytes[j] = (kHexLookup[static_cast<uint8_t>(c1)] << 4) |
                kHexLookup[static_cast<uint8_t>(c2)];
        }

        return true;
    }
} // namespace esp32_c3::utils
