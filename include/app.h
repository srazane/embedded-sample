#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stdint.h>

enum {
    APP_BLINK_INTERVAL_MS = 500U,
    APP_BUTTON_DEBOUNCE_MS = 25U
};

typedef struct {
    uint32_t last_blink_ms;
    uint32_t button_changed_ms;
    bool led_on;
    bool blinking_enabled;
    bool button_raw;
    bool button_stable;
} app_state_t;

void app_init(app_state_t *app, uint32_t now_ms, bool button_pressed);
bool app_update(app_state_t *app, uint32_t now_ms, bool button_pressed);

#endif
