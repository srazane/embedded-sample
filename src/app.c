#include "app.h"

void app_init(app_state_t *app, uint32_t now_ms, bool button_pressed) {
    app->last_blink_ms = now_ms;
    app->button_changed_ms = now_ms;
    app->led_on = false;
    app->blinking_enabled = true;
    app->button_raw = button_pressed;
    app->button_stable = button_pressed;
}

bool app_update(app_state_t *app, uint32_t now_ms, bool button_pressed) {
    if (button_pressed != app->button_raw) {
        app->button_raw = button_pressed;
        app->button_changed_ms = now_ms;
    }

    if (app->button_raw != app->button_stable &&
        (uint32_t)(now_ms - app->button_changed_ms) >= APP_BUTTON_DEBOUNCE_MS) {
        app->button_stable = app->button_raw;
        if (app->button_stable) {
            app->blinking_enabled = !app->blinking_enabled;
            app->last_blink_ms = now_ms;
            if (!app->blinking_enabled) {
                app->led_on = false;
            }
        }
    }

    if (app->blinking_enabled &&
        (uint32_t)(now_ms - app->last_blink_ms) >= APP_BLINK_INTERVAL_MS) {
        app->last_blink_ms = now_ms;
        app->led_on = !app->led_on;
    }

    return app->led_on;
}
