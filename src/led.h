#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_toggle(void);
int led_get_state(void);

#endif
