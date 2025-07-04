#include "esp32_c3_objects/led.h"
#include <esp_log.h>

namespace esp32_c3_objects
{
    Led::Led(const gpio_num_t pin) noexcept
    {
        init(pin);
    }

    void Led::init(const gpio_num_t pin) noexcept
    {
        if (mPin == GPIO_NUM_NC && pin != GPIO_NUM_NC)
        {
            mPin = pin;
            pinMode(mPin, OUTPUT);
            digitalWrite(mPin, HIGH);
            log_d("Initialized pin %d", mPin);
        }
    }

    void Led::setMode(const LedMode mode) noexcept
    {
        mMode = mode;
        mStep = 0;
        mNextUpdate = 0;
        updateOutput(); // Немедленное обновление состояния
        log_i("Mode changed to %d", static_cast<int>(mode));
    }

    void Led::update(uint32_t currentTime) noexcept
    {
        if (mPin == GPIO_NUM_NC) return;

        if (currentTime == 0)
        {
            currentTime = millis();
        }

        if (currentTime < mNextUpdate)
        {
            return;
        }

        uint16_t timeout = blinkInterval;

        switch (mMode)
        {
        case LedMode::ON:
            mIsOn = true;
            break;

        case LedMode::BLINK:
            mIsOn = !mIsOn;
            mStep = mIsOn ? 1 : 0;
            break;

        case LedMode::DOUBLE_BLINK:
            mIsOn = (mStep % 2) == 0;
            mStep = (mStep + 1) % 4;
            timeout = blinkInterval / 2;
            break;

        case LedMode::TRIPLE_BLINK:
            mIsOn = (mStep % 2) == 0;
            mStep = (mStep + 1) % 6;
            timeout = blinkInterval / 3;
            break;

        default: // OFF
            mIsOn = false;
            break;
        }

        updateOutput();
        mNextUpdate = currentTime + timeout;
    }

    void Led::updateOutput() const noexcept
    {
        digitalWrite(mPin, mIsOn ? LOW : HIGH);
        log_d("Pin %d set to %s", mPin, mIsOn ? "LOW" : "HIGH");
    }
} // namespace esp32_c3_utils
