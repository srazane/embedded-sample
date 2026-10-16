#include <assert.h>
#include <stdio.h>

#include "led.h"

int main(void) {
    led_init();
    assert(led_get_state() == 0);

    led_toggle();
    assert(led_get_state() == 1);

    led_toggle();
    assert(led_get_state() == 0);

    puts("TESTS PASSED");
    return 0;
}
