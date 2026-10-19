#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app.h"

static unsigned int checks;

#define CHECK(condition)                                                        \
    do {                                                                        \
        ++checks;                                                               \
        if (!(condition)) {                                                     \
            fprintf(stderr, "FAIL: %s (line %d)\n", #condition, __LINE__);      \
            return 1;                                                           \
        }                                                                       \
    } while (0)

static int test_blink_timing(void) {
    app_state_t app;

    app_init(&app, 100U, false);
    CHECK(!app_update(&app, 599U, false));
    CHECK(app_update(&app, 600U, false));
    CHECK(!app_update(&app, 1100U, false));

    return 0;
}

static int test_button_debounce_and_toggle(void) {
    app_state_t app;

    app_init(&app, 0U, false);
    CHECK(!app_update(&app, 10U, true));
    CHECK(!app_update(&app, 34U, true));
    CHECK(!app_update(&app, 35U, true));
    CHECK(!app.blinking_enabled);
    CHECK(!app_update(&app, 60U, false));
    CHECK(!app_update(&app, 85U, false));
    CHECK(!app.blinking_enabled);

    CHECK(!app_update(&app, 100U, true));
    CHECK(app_update(&app, 125U, true) == false);
    CHECK(app.blinking_enabled);
    CHECK(!app_update(&app, 624U, true));
    CHECK(app_update(&app, 625U, true));

    return 0;
}

static int test_button_bounce_restarts_debounce(void) {
    app_state_t app;

    app_init(&app, 0U, false);
    (void)app_update(&app, 10U, true);
    (void)app_update(&app, 20U, false);
    (void)app_update(&app, 25U, true);
    (void)app_update(&app, 49U, true);
    CHECK(app.blinking_enabled);
    (void)app_update(&app, 50U, true);
    CHECK(!app.blinking_enabled);

    return 0;
}

static int test_millisecond_counter_wraparound(void) {
    app_state_t app;
    const uint32_t start = UINT32_MAX - 200U;

    app_init(&app, start, false);
    CHECK(!app_update(&app, start + 499U, false));
    CHECK(app_update(&app, start + 500U, false));

    return 0;
}

int main(void) {
    if (test_blink_timing() != 0 ||
        test_button_debounce_and_toggle() != 0 ||
        test_button_bounce_restarts_debounce() != 0 ||
        test_millisecond_counter_wraparound() != 0) {
        return 1;
    }

    printf("PASS: %u application checks\n", checks);
    return 0;
}
