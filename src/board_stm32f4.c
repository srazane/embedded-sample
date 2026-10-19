#include "board.h"

#include "stm32f4xx_hal.h"

#define USER_LED_GPIO_PORT GPIOA
#define USER_LED_PIN GPIO_PIN_5
#define USER_BUTTON_GPIO_PORT GPIOC
#define USER_BUTTON_PIN GPIO_PIN_13

void board_init(void) {
    RCC_OscInitTypeDef oscillator_config = {0};
    RCC_ClkInitTypeDef clock_config = {0};
    GPIO_InitTypeDef gpio_config = {0};

    oscillator_config.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    oscillator_config.HSIState = RCC_HSI_ON;
    oscillator_config.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscillator_config.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&oscillator_config) != HAL_OK) {
        Error_Handler();
    }

    clock_config.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                             RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock_config.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clock_config.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock_config.APB1CLKDivider = RCC_HCLK_DIV1;
    clock_config.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock_config, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    gpio_config.Pin = USER_LED_PIN;
    gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(USER_LED_GPIO_PORT, &gpio_config);
    HAL_GPIO_WritePin(USER_LED_GPIO_PORT, USER_LED_PIN, GPIO_PIN_RESET);

    gpio_config.Pin = USER_BUTTON_PIN;
    gpio_config.Mode = GPIO_MODE_INPUT;
    gpio_config.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USER_BUTTON_GPIO_PORT, &gpio_config);
}

bool board_button_pressed(void) {
    return HAL_GPIO_ReadPin(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN) == GPIO_PIN_SET;
}

void board_led_set(bool on) {
    HAL_GPIO_WritePin(USER_LED_GPIO_PORT, USER_LED_PIN,
                      on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void HAL_MspInit(void) {
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

void Error_Handler(void) {
    __disable_irq();
    for (;;) {
    }
}
