#ifndef ESP32_C3_TYPE_UTILS_H
#define ESP32_C3_TYPE_UTILS_H

#include <array>
#include <cstdio>
#include <cstring>
#include <esp_log.h>

namespace esp32_c3::utils
{
    /**
     * @brief Генерация тега для логов на основе типа
     * @tparam T Тип для которого генерируется тег
     * @return constexpr std::array<char, N> с текстом тега
     */
    template <typename T>
    constexpr auto generateTag(const char* className)
    {
        constexpr auto typeName =
#ifdef _MSC_VER
            __FUNCSIG__;
#else
            __PRETTY_FUNCTION__;
#endif

        // Ищем начало типа в строке
        const char* typeStart = strstr(typeName, "T = ") + 4;
        const char* typeEnd = strchr(typeStart, ';');
        if (!typeEnd) typeEnd = strchr(typeStart, ']');
        if (!typeEnd) typeEnd = typeStart + strlen(typeStart);

        // Вычисляем длину
        const size_t typeLen = typeEnd - typeStart;

        // Создаем массив для хранения
        std::array<char, 64> result{};
        snprintf(result.data(), result.size(), "%s<%.*s>",
                 className, static_cast<int>(typeLen), typeStart);

        return result;
    }
} // namespace esp32_c3::utils

#endif //ESP32_C3_TYPE_UTILS_H
