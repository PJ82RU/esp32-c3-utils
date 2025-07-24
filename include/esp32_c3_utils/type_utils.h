#ifndef ESP32_C3_TYPE_UTILS_H
#define ESP32_C3_TYPE_UTILS_H

#include <cstdio>
#include <cstring>
#include <esp_log.h>

namespace esp32_c3::utils
{
    /**
     * @brief Генерация тега для логов на основе типа
     * @tparam T Тип для которого генерируется тег
     * @return constexpr указатель на строку с текстом тега
     */
    template <typename T>
    constexpr const char* generateTag(const char* className)
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

        // Вычисляем длину типа
        const size_t typeLen = typeEnd - typeStart;
        const size_t classNameLen = strlen(className);

        // Статический буфер для каждого специализированного типа
        static char buffer[64] = {};

        // Формируем строку тега
        snprintf(buffer, sizeof(buffer), "%s<%.*s>",
                 className, static_cast<int>(typeLen), typeStart);

        return buffer;
    }
} // namespace esp32_c3::utils

#endif // ESP32_C3_TYPE_UTILS_H
