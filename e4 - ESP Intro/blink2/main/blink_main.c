#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define DELAY1 (1000 / portTICK_PERIOD_MS)
#define DELAY2 (2000 / portTICK_PERIOD_MS)
#define PIN1 (33)
#define PIN2 (32)

void TaskBlink1 (void *pvParameters)
{
    uint8_t state = 1;
    while (1) {
        gpio_set_level(PIN1, state);
        vTaskDelay(DELAY1);
        state = !state;
    }
}

void TaskBlink2 (void *pvParameters)
{
    uint8_t state = 1;
    while (1) {
        gpio_set_level(PIN2, state);
        vTaskDelay(DELAY2);
        state = !state;
    }
}

void app_main(void)
{
    gpio_reset_pin(PIN1);
    gpio_set_direction(PIN1, GPIO_MODE_OUTPUT);
    xTaskCreate(TaskBlink1, "Blink1", 4096, NULL, 1, NULL);

    gpio_reset_pin(PIN2);
    gpio_set_direction(PIN2, GPIO_MODE_OUTPUT);
    xTaskCreate(TaskBlink2, "Blink2", 4096, NULL, 1, NULL);
}