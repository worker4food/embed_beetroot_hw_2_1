#pragma once

#include <driver/gpio.h>

class Cfg {
    public:
        static constexpr gpio_num_t ButtonPin = GPIO_NUM_47;
        static constexpr gpio_num_t LedPin = GPIO_NUM_2;

        static const int64_t debounce_delay_us = 5000;
        static const int64_t blink_delay_us = 200000;
};
