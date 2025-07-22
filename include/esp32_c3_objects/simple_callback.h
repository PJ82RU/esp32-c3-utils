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
        explicit SimpleCallback(const CallbackFunction callback = nullptr, P params = P{}) noexcept
            : mCallback(callback),
              mParams(params)
        {
        }

        /**
         * @brief Установка callback
         * @param callback Функция обратного вызова
         * @param params Пользовательские параметры
         */
        void set(CallbackFunction callback, P params) noexcept
        {
            mCallback = callback;
            mParams = params;
        }

        /**
         * @brief Сброс зарегистрированного callback
         * @note После вызова isSet() будет возвращать false
         */
        void reset() noexcept
        {
            mCallback = nullptr;
            mParams = P{};
        }

        /**
         * @brief Вызов зарегистрированного callback
         * @param value Указатель на данные для передачи в callback
         * @note Ничего не происходит если callback не установлен
         */
        void invoke(const T& value) const noexcept
        {
            if (mCallback != nullptr)
            {
                mCallback(value, mParams);
            }
        }

        /**
         * @brief Проверка наличия зарегистрированного callback
         * @return true если callback был установлен и не был сброшен
         */
        [[nodiscard]] bool isSet() const noexcept
        {
            return mCallback != nullptr;
        }

    private:
        /// Указатель на функцию обратного вызова
        CallbackFunction mCallback = nullptr;

        /// Пользовательские параметры для callback
        P mParams = nullptr;
    };
} // namespace esp32_c3::objects

#endif // ESP32_C3_UTILS_SIMPLE_CALLBACK_H