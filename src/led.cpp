#include "esp32_c3_objects/led.h"


namespace esp32_c3::objects
{
    Led::Led(const gpio_num_t pin) noexcept
    {
        init(pin);
    }

    void Led::init(const gpio_num_t pin) noexcept
    {
        if (pin != mPin && pin != GPIO_NUM_NC)
        {
            if (mPin != GPIO_NUM_NC)
            {
                // Освобождаем предыдущий пин если был инициализирован
                gpio_reset_pin(mPin);
            }

            mPin = pin;
            gpio_set_direction(mPin, GPIO_MODE_OUTPUT);
            gpio_set_level(mPin, 1); // Выключенное состояние (активный низкий)
            ESP_LOGD(TAG, "LED initialized on pin %d", mPin);
        }
    }

    void Led::setMode(LedMode mode) noexcept
    {
        if (mMode != mode)
        {
            mMode = mode;
            mStep = 0;
            mNextUpdate = 0;
            updateOutput();
            ESP_LOGI(TAG, "Mode changed to %d", static_cast<int>(mode));
        }
    }

    void Led::update(const uint64_t currentTime) noexcept
    {
        if (mPin == GPIO_NUM_NC || mMode == LedMode::OFF || mMode == LedMode::ON) return;
        if (currentTime < mNextUpdate) return;

        uint64_t timeout = msToUs(blinkInterval);

        switch (mMode)
        {
        case LedMode::BLINK:
            mIsOn = !mIsOn;
            mStep = mIsOn ? 1 : 0;
            break;

        case LedMode::DOUBLE_BLINK:
            mIsOn = (mStep % 2) == 0;
            mStep = (mStep + 1) % 4;
            timeout = msToUs(blinkInterval) / 2;
            break;

        case LedMode::TRIPLE_BLINK:
            mIsOn = (mStep % 2) == 0;
            mStep = (mStep + 1) % 6;
            timeout = msToUs(blinkInterval) / 3;
            break;

        default:
            // Обработано в начале метода
            break;
        }

        updateOutput();
        mNextUpdate = currentTime + timeout;
    }

    void Led::updateOutput() const noexcept
    {
        if (mPin != GPIO_NUM_NC)
        {
            gpio_set_level(mPin, mIsOn ? 0 : 1); // Активный низкий уровень
            ESP_LOGD(TAG, "Pin %d set to %s", mPin, mIsOn ? "ON" : "OFF");
        }
    }
} // namespace esp32_c3::objects
