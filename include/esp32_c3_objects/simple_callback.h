#ifndef ESP32_C3_UTILS_SIMPLE_CALLBACK_H
#define ESP32_C3_UTILS_SIMPLE_CALLBACK_H

#include <type_traits>

namespace esp32_c3::objects
{
    /**
     * @brief Типизированный класс для работы с callback-функциями
     * @tparam T Тип передаваемых данных
     * @tparam P Тип пользовательских параметров (по умолчанию void*)
     */
    template <typename T, typename P = void*>
    class SimpleCallback
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Type T must be trivially copyable");

    public:
        /**
         * @brief Тип функции обратного вызова
         * @param value Указатель на передаваемые данные
         * @param params Пользовательские параметры
         */
        using CallbackFunction = void (*)(const T& value, P params) noexcept;

        /**
         * @brief Конструктор
         * @param callback Функция обратного вызова (может быть nullptr)
         * @param params Пользовательские параметры
         */
        explicit SimpleCallback(CallbackFunction callback = nullptr, P params = P{}) noexcept;

        /**
         * @brief Установка callback
         * @param callback Функция обратного вызова
         * @param params Пользовательские параметры
         */
        void set(CallbackFunction callback, P params) noexcept;

        /**
         * @brief Сброс зарегистрированного callback
         * @note После вызова isSet() будет возвращать false
         */
        void reset() noexcept;

        /**
         * @brief Вызов зарегистрированного callback
         * @param value Указатель на данные для передачи в callback
         * @note Ничего не происходит если callback не установлен
         */
        void invoke(const T& value) const noexcept;

        /**
         * @brief Проверка наличия зарегистрированного callback
         * @return true если callback был установлен и не был сброшен
         */
        [[nodiscard]] bool isSet() const noexcept;

    private:
        /// Указатель на функцию обратного вызова
        CallbackFunction mCallback = nullptr;

        /// Пользовательские параметры для callback
        P mParams = nullptr;
    };
} // namespace esp32_c3::utils

#endif // ESP32_C3_UTILS_SIMPLE_CALLBACK_H
