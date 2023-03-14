#include <stdio.h>
#include "driver/adc.h"

uint32_t sample(adc1_channel_t channel)
{
    return adc1_get_raw(channel);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);

    while (1) {
        uint32_t adc_reading = sample(ADC1_CHANNEL_5);
        printf("ADC reading: %ld\n", adc_reading);
    }
}
