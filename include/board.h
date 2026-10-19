#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

void board_init(void);
bool board_button_pressed(void);
void board_led_set(bool on);
void Error_Handler(void);

#endif
