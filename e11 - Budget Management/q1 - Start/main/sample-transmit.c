#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

int sample(adc1_channel_t channel)
{
    return adc1_get_raw(channel);
}

void transmit(int reading)
{
    printf("%u\n", reading);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_11);

    while (1)
    {
        int reading = sample(ADC_CHANNEL_6);
        transmit(reading);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
