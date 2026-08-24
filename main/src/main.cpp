
#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_log.h>
#include <esp_check.h>
#include <esp_task_wdt.h>

#include "cfg.hpp"
#include "wired_led.hpp"
#include "wired_button.hpp"

// gpio02 -- [ 220 Ohm ] -- [A(+) LED2 K(-)] -- GND
// gpio47 -- [ _/  BTN ] -- GND

volatile int button_state = 0;

void IRAM_ATTR on_button(void *) {
    button_state = gpio_get_level(Cfg::ButtonPin);
}

esp_err_t setup_isr_handler() {
    ESP_RETURN_ON_ERROR(gpio_install_isr_service(0), __func__, "gpio_install_isr_service error");

    return gpio_isr_handler_add(Cfg::ButtonPin, on_button, nullptr);
}

extern "C" {

void app_main() {
    auto led = WiredLed(Cfg::LedPin, Cfg::blink_delay_us);
    auto btn = WiredButton(Cfg::ButtonPin, 0, Cfg::debounce_delay_us);

    ESP_RETURN_VOID_ON_ERROR(led.init(),  __func__, "led (%d) init error", Cfg::LedPin);
    ESP_RETURN_VOID_ON_ERROR(btn.init(),  __func__, "pushbutton (%d) init error", Cfg::ButtonPin);
    ESP_RETURN_VOID_ON_ERROR(setup_isr_handler(), __func__, "setup_isr_handler error");

    esp_task_wdt_deinit(); // superloop

    int64_t iters = 0;
    int64_t start_us;
    constexpr auto max_iterations = 100000u;

    start_us = esp_timer_get_time();
    for(;;) {
        auto current_us = esp_timer_get_time();

        if (btn.check_event(button_state, current_us) == WiredButton::Event::Released) {
            auto next = led.switch_state();
            ESP_LOGI(__func__, "Led state: %d", next);
        }

        led.loop(current_us);

        if (++iters >= max_iterations) {
            double iter_time_us = (current_us - start_us + 0.0)/iters;
            ESP_LOGI(__func__, "avg iteration time over %lld iterations: %f µs", iters, iter_time_us);

            start_us = current_us;
            iters = 0;
        }
    }
}

}
