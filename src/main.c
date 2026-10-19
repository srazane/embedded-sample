#include "app.h"
#include "board.h"

#include "stm32f4xx_hal.h"

int main(void) {
    app_state_t app;

    HAL_Init();
    board_init();
    app_init(&app, HAL_GetTick(), board_button_pressed());

    for (;;) {
        const bool led_on =
            app_update(&app, HAL_GetTick(), board_button_pressed());
        board_led_set(led_on);
        __WFI();
    }
}
