#pragma once

#include <esp_check.h>
#include <driver/gpio.h>

enum class Led {
    Off = 0, On = 1, Blink = 2
};

constexpr uint8_t TotalStates = 3;

class WiredLed {
    protected:
        Led state;
        int64_t last_toggle_us = 0;

    public:
        const gpio_num_t pin;
        const int64_t blink_delay_us;

        WiredLed(gpio_num_t gpio_pin, int64_t blink_delay_us): pin(gpio_pin), blink_delay_us(blink_delay_us) {}

        esp_err_t init(Led s = Led::Off) {
            state = s;

            ESP_RETURN_ON_ERROR(gpio_reset_pin(pin), __func__, "reset pin failed");

            return gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
        }

        esp_err_t loop(int64_t current_us) {
            auto current = gpio_get_level(pin);
            esp_err_t res = ESP_OK;

            if (Led::Off == state && 1 == current) {
                res = gpio_set_level(pin, 0);
            } else if (Led::On == state && 0 == current) {
                res = gpio_set_level(pin, 1);
            } else if (Led::Blink == state && current_us >= (last_toggle_us + blink_delay_us)) {
                last_toggle_us = current_us;
                res = gpio_set_level(pin, 1 - current);
            }

            return res;
        }

        Led switch_state() {
            auto next = (static_cast<int8_t>(state) + 1) % TotalStates;
            state = static_cast<Led>(next);

            return state;
        }
};
