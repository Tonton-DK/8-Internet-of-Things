#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define DEFAULT_VREF    3300.0  // Default ADC reference voltage in mV
#define LMT86_GAIN      -10.9   // Average sensor gain of the LMT86 temperature sensor
#define LMT86_OFFSET    2103.0  // Offset voltage of the LMT86 temperature sensor

uint32_t sample(adc1_channel_t channel)
{
    return adc1_get_raw(channel);
}

uint32_t convert(uint32_t raw)
{
    // Convert ADC reading to temperature in degrees Celsius
    float voltage = (float)raw * DEFAULT_VREF / 4095.0;
    //printf("Voltage: %.2f \n", voltage);
    float temperature = (voltage - LMT86_OFFSET) / LMT86_GAIN;
    return temperature;
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);

    //printf("---\n");
    
    while (1) {
        // Read ADC value
        uint32_t input = sample(ADC1_CHANNEL_5);
        //printf("Sample: %ld \n", input);

        // Convert ADC value
        float temperature = convert(input);

        // Print temperature value
        //printf("Temperature: %.2f C\n", temperature);
        printf("%.2f\n", temperature);
        
        //printf("---\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // introduce a delay of 1 second
    }
}
