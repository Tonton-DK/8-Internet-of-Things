#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define DELAY1 (1000 / portTICK_PERIOD_MS)
#define PIN1 (33)

void TaskRead (void *pvParameters)
{
    uint8_t state = 1;
    while (1) {
        uint8_t stateNew = gpio_get_level(PIN1);
        if(state != stateNew)
        {
            state = stateNew;
            printf("State: %u\n", state);
        }
        vTaskDelay(DELAY1);
    }
}

void app_main(void)
{
    gpio_reset_pin(PIN1);
    gpio_set_direction(PIN1, GPIO_MODE_INPUT);
    xTaskCreate(TaskRead, "Read", 4096, NULL, 1, NULL);
}