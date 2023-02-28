#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define DELAY (1000 / portTICK_PERIOD_MS)
#define PIN (33)

void TaskBlink (void *pvParameters)
{
    uint8_t state = 1;
    while (1) {
        gpio_set_level(PIN, state);
        vTaskDelay(DELAY);
        state = !state;
    }
}

void app_main(void)
{
    gpio_reset_pin(PIN);
    gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
    xTaskCreate(TaskBlink, "Blink", 4096, NULL, 1, NULL);
}