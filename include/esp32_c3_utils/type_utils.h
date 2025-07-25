#ifndef ESP32_C3_TYPE_UTILS_H
#define ESP32_C3_TYPE_UTILS_H

#include <cstring>

namespace esp32_c3::utils
{
#ifndef DISABLE_DEBUG_TAGS
    template <typename Class>
    constexpr const char* generateTag() noexcept
    {
#ifdef __GNUC__
        constexpr auto prefix = "Class = ";
        constexpr auto suffix = "]";
        const auto pretty = __PRETTY_FUNCTION__;

        const char* start = strstr(pretty, prefix);
        if (!start) return "[Error]";
        start += strlen(prefix);

        const char* end = strstr(start, suffix);
        if (!end) return "[Error]";

        static char buffer[128];
        size_t len = end - start;
        len = len < sizeof(buffer) - 3 ? len : sizeof(buffer) - 3;
        strncpy(buffer + 1, start, len);
        buffer[0] = '[';
        buffer[len + 1] = ']';
        buffer[len + 2] = '\0';

        if (const char* lastColon = strrchr(buffer, ':'))
        {
            if (lastColon[1] == ':')
            {
                memmove(buffer + 1, lastColon + 2, strlen(lastColon + 2) + 1);
                len = strlen(buffer + 1);
                buffer[len + 1] = ']';
                buffer[len + 2] = '\0';
            }
        }

        return buffer;
#else
        return "[Type]";
#endif
    }
#else
    // Упрощенная версия для релиза
    template <typename Class>
    constexpr const char* generateTag() noexcept {
        return "[Class]";
    }
#endif // DISABLE_DEBUG_TAGS
} // namespace esp32_c3::utils

#endif // ESP32_C3_TYPE_UTILS_H
