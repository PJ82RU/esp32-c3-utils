#include "esp32_c3_utils/system_info.h"

#include <charconv>
#include <string_view>
#include <cstring>

// ESP-IDF заголовки
#include <algorithm>
#include "esp_app_desc.h"
#include "esp_chip_info.h"
#include "esp_cpu.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_ota_ops.h"
#include "esp_rom_sys.h"
#include "esp_system.h"
#include "esp_private/esp_clk.h"

namespace esp32_c3::utils
{
    namespace
    {
        /**
         * @brief Безопасное преобразование строки в число
         * @param str Строка для преобразования
         * @param[out] out_val Результат преобразования
         * @return true если преобразование успешно
         */
        bool SafeStringToUint(const std::string_view str, uint32_t& out_val) noexcept
        {
            const auto* end = str.data() + str.size();
            auto [ptr, ec] = std::from_chars(str.data(), end, out_val);
            return (ec == std::errc() && ptr != str.data());
        }
    }

    bool getSystemInfo(SystemInfo& info) noexcept
    {
        // Инициализация нулями через value-initialization
        info = SystemInfo{};

        // 1. Получение MAC-адреса
        esp_efuse_mac_get_default(info.macArray.data());

        // 2. Информация о чипе
        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);
        info.chipCores = chip_info.cores;
        info.chipRevision = chip_info.revision;
        snprintf(info.chipModel.data(), info.chipModel.size(), "ESP32-C3");

        // 3. Частота CPU
        info.cpuFreqMhz = esp_clk_cpu_freq() / 1000000;

        // 4. Счетчик циклов
        info.cycleCount = esp_cpu_get_cycle_count();

        // 5. Flash память
        esp_flash_t* flash = esp_flash_default_chip;
        esp_flash_get_size(flash, &info.flashChipSize);

        // Безопасное преобразование скорости Flash
        if (!SafeStringToUint(CONFIG_ESPTOOLPY_FLASHFREQ, info.flashChipSpeed))
        {
            info.flashChipSpeed = 80; // Значение по умолчанию
            ESP_LOGW("Flash", "Invalid speed, using default 80MHz");
        }

        // 6. Память
        info.freeHeap = esp_get_free_heap_size();
        info.heapSize = esp_get_minimum_free_heap_size();

        // 7. Информация о приложении
        if (const auto* app_desc = esp_app_get_description())
        {
            if (const auto* running = esp_ota_get_running_partition())
            {
                info.sketchSize = running->size;
            }
            std::copy_n(app_desc->app_elf_sha256, info.sketchMd5.size(),
                        info.sketchMd5.begin());
            std::strncpy(info.sdkVersion.data(), app_desc->idf_ver,
                         info.sdkVersion.size());
        }

        ESP_LOGI("SysInfo", "CPU: %" PRIu32 "MHz, Flash: %" PRIu32 "MB",
                 info.cpuFreqMhz, info.flashChipSize / (1024 * 1024));

        return true;
    }
} // namespace esp32_c3::utils
