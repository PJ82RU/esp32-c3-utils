#include "esp32_c3_utils/usr_data.h"
#include <esp_efuse.h>
#include <esp_efuse_table.h>
#include <cstring>
#include <array>
#include <esp32-hal-log.h>

namespace esp32_c3_utils
{
    std::string readUserData() noexcept
    {
        std::array<char, EFUSE_USER_DATA_SIZE> buffer{};

        // Чтение данных (размер в битах!)
        esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA,
                                  buffer.data(),
                                  buffer.size() * 8);

        return {buffer.data(), strnlen(buffer.data(), buffer.size())};
    }

    bool writeUserData(const std::string& data) noexcept
    {
        if (data.empty() || data.size() > EFUSE_USER_DATA_SIZE)
        {
            log_e("EFUSE", "Invalid data size: %zu", data.size());
            return false;
        }

        if (isUserDataWritten())
        {
            log_w("EFUSE", "USER_DATA already written");
            return false;
        }

        // Запись данных (размер указывается в битах!)
        const esp_err_t err = esp_efuse_write_field_blob(
            ESP_EFUSE_USER_DATA,
            data.data(),
            data.size() * 8);

        if (err != ESP_OK)
        {
            log_e("EFUSE", "Write failed: 0x%X", err);
            return false;
        }

        log_i("EFUSE", "Written %zu bytes to USER_DATA", data.size());
        return true;
    }

    bool isUserDataWritten() noexcept
    {
        return esp_efuse_read_field_bit(ESP_EFUSE_USER_DATA);
    }
} // namespace esp32_c3_utils
