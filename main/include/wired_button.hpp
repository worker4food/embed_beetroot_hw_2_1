#pragma once

#include <driver/gpio.h>

class WiredButton {
    protected:
        int level, unstable_level = -1;
        std::int64_t switched_at_us = -1;
    public:
        const gpio_num_t pin;
        const std::uint32_t pressed_level;
        const std::uint64_t debounce_delay_us;

        enum class Event {
            Pressed, Released, None
        };

        WiredButton(gpio_num_t gpio_pin, std::uint32_t pressed_lvl, std::int64_t debounce_interval_us)
            :pin(gpio_pin), pressed_level(pressed_lvl), debounce_delay_us(debounce_interval_us)
        {
            level = -1;
            switched_at_us = -1;
        }

        esp_err_t init() {
            gpio_config_t b = {
                .pin_bit_mask = 1ull << pin,
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = pressed_level == 0 ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
                .pull_down_en = pressed_level == 1 ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_ANYEDGE
            };

            return gpio_config(&b);
        }

        Event check_event(int new_level, std::int64_t current_us) {
            if (unstable_level != new_level) {

                unstable_level = new_level;
                switched_at_us = current_us;

                return Event::None;
            } else if (debounce_delay_us < (current_us - switched_at_us) && new_level != level) {

                switched_at_us = current_us;
                level = new_level;
                unstable_level = new_level;

                if (pressed_level == new_level) {
                    return Event::Pressed;
                } else {
                    return Event::Released;
                }
            }

            return Event::None;
        }
};
