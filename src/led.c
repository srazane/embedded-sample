#include "led.h"

/* Simulated LED state for host-side tests. */
static int led_state = 0;

void led_init(void) {
    led_state = 0;
}

void led_toggle(void) {
    led_state = !led_state;
}

int led_get_state(void) {
    return led_state;
}
