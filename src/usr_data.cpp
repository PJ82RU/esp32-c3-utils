#include "esp32_c3_utils/usr_data.h"

#include <array>

#include <algorithm>
#include <esp_efuse.h>
#include <esp_efuse_table.h>
#include <esp_log.h>

namespace
{
    constexpr auto TAG = "eFuse"; ///< Тег для логирования
}

namespace esp32_c3::utils
{
    std::string readUserData() noexcept
    {
        std::array<uint8_t, EFUSE_USER_DATA_SIZE> buffer{};

        esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA,
                                  buffer.data(),
                                  buffer.size() * 8);

        const auto it = std::ranges::find(buffer, uint8_t{0});
        return std::string{
            reinterpret_cast<const char*>(buffer.data()),
            static_cast<size_t>(std::distance(buffer.begin(), it))
        };
    }

    bool writeUserData(const std::string& data) noexcept
    {
        if (data.empty() || data.size() > EFUSE_USER_DATA_SIZE)
        {
            ESP_LOGE(TAG, "Недопустимый размер данных: %zu", data.size());
            return false;
        }

        if (isUserDataWritten())
        {
            ESP_LOGW(TAG, "USER_DATA уже записан");
            return false;
        }

        // Запись данных (размер в битах)
        const esp_err_t err = esp_efuse_write_field_blob(
            ESP_EFUSE_USER_DATA,
            data.data(),
            data.size() * 8);

        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Ошибка записи: 0x%X", err);
            return false;
        }

        ESP_LOGI(TAG, "Записано %zu байт в USER_DATA", data.size());
        return true;
    }

    bool isUserDataWritten() noexcept
    {
        return esp_efuse_read_field_bit(ESP_EFUSE_USER_DATA);
    }
} // namespace esp32_c3::utils
