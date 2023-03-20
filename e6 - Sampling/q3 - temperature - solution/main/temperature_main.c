#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

// https://embeddedexplorer.com/esp32-adc-esp-idf-tutorial/

#define V1 (1.993)
#define T1 (10)
#define V2 (1.668)
#define T2 (40)

esp_adc_cal_characteristics_t adc1_chars;

float raw2voltage (int reading) {
    int mv = esp_adc_cal_raw_to_voltage(reading, &adc1_chars);
    return (float)mv/1000;
}

float voltage2temp (float voltage) {
    return (voltage-V1)/((V2-V1)/(T2-T1))+T1;
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc1_chars);

    while (1) {
        int reading = adc1_get_raw((adc1_channel_t)ADC1_CHANNEL_5);
        float voltage = raw2voltage(reading);
        float temp = voltage2temp(voltage);
        printf("%u -> %f V -> %f C\n", reading, voltage, temp);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
