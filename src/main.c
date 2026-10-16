#include "led.h"

/* Minimal embedded-style application that calls the HAL API. */
int main(void) {
    led_init();
    for (int i = 0; i < 10; ++i) {
        led_toggle();
        /* A real firmware would wait here and toggle a physical LED. */
    }
    return 0;
}
