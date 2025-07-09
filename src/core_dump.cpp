#include "esp32_c3_utils/core_dump.h"
#include "esp_core_dump.h"
#include "esp_log.h"

namespace esp32_c3::utils {

    void checkAndFixCoreDump() noexcept
    {
        // Проверяем наличие core dump в flash
        size_t dump_size = 0;
        esp_err_t err = esp_core_dump_image_get(&dump_size, nullptr);

        if (err == ESP_OK) {
            ESP_LOGI("CoreDump", "Valid core dump found (%zu bytes)", dump_size);
            return;
        }

        ESP_LOGW("CoreDump", "Core dump check failed (0x%X): %s",
                 err, esp_err_to_name(err));

        if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGI("CoreDump", "No core dump found - this is normal for first boot");
            return;
        }

        // Пытаемся удалить поврежденный core dump
        ESP_LOGI("CoreDump", "Attempting to erase corrupted core dump...");
        err = esp_core_dump_image_erase();

        if (err != ESP_OK) {
            ESP_LOGE("CoreDump", "Failed to erase core dump: 0x%X: %s",
                    err, esp_err_to_name(err));
        } else {
            ESP_LOGI("CoreDump", "Corrupted core dump erased successfully");
        }
    }

} // namespace esp32_c3::utils