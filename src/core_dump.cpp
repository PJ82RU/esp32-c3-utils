#include "esp32_c3_utils/core_dump.h"
#include <esp_core_dump.h>
#include <esp32-hal.h>

namespace esp32_c3_utils
{
    void checkAndFixCoreDump() noexcept
    {
        // Инициализация core dump
        esp_core_dump_init();

        // Выделение памяти под структуру
        auto* summary = static_cast<esp_core_dump_summary_t*>(
            malloc(sizeof(esp_core_dump_summary_t))
        );

        if (!summary)
        {
            log_e("Failed to allocate memory for core dump summary");
            return;
        }

        // Попытка прочитать core dump
        const esp_err_t err = esp_core_dump_get_summary(summary);
        if (err != ESP_OK)
        {
            log_w("Core dump corrupted or not found (0x%X), attempting repair...", err);

            // Удаление поврежденного дампа
            if (esp_core_dump_image_erase() == ESP_OK)
            {
                log_d("Creating new core dump...");

                // Создание нового дампа
                panic_info_t panicInfo;
                esp_core_dump_to_flash(&panicInfo);

                log_i("Core dump successfully repaired");
            }
            else
            {
                log_e("Failed to erase corrupted core dump");
            }
        }
        else
        {
            log_i("Valid core dump found, no repair needed");
        }

        // Освобождение памяти
        free(summary);
    }
} // namespace esp32_c3_utils
