#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define DELAY(ms) ((ms) / portTICK_PERIOD_MS)
#define PIN (33)

typedef struct {
  gpio_num_t pin;
  TickType_t delay;
} params_t;

params_t config1 = {33, DELAY(1000)};
params_t config2 = {32, DELAY( 800)};

void TaskBlink (void *pvParameters)
{
    params_t* params = (params_t*) pvParameters;
    
    // pin setup
    gpio_reset_pin(params->pin);
    gpio_set_direction(params->pin, GPIO_MODE_OUTPUT);
    
    uint8_t state = 1;
    while (1) {
        gpio_set_level(params->pin, state);
        vTaskDelay(params->delay);
        state = !state;
    }
}

void app_main(void)
{
    xTaskCreate(TaskBlink, "Blink", 4096, &config1, 1, NULL);
    xTaskCreate(TaskBlink, "Blink", 4096, &config2, 1, NULL);
}
