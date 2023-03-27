#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hl_wifi.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "string.h"

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

void TaskTemperature(void *pvParameters)
{
    printf("Callback reached!\n");
    char *ip = "192.168.1.105";
    uint16_t host = 8000;
    sockaddr_in_t addr = hl_wifi_make_addr(ip, host);
    int sock = hl_wifi_tcp_connect(addr);
    if (sock == -1)
    {
        printf("Connection failure\n");
        vTaskDelete(NULL);
    }

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);

    char buffer[16];
    while (true) 
    {
        // Read ADC value
        uint32_t input = sample(ADC1_CHANNEL_5);
        // Convert ADC value
        float temperature = convert(input);
        // write the temperature to the buffer
        sprintf(buffer, "%.2f C\n", temperature);
        uint16_t length = strlen(buffer);
        hl_wifi_tcp_tx(sock, buffer, length);
        // introduce a delay of 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void connected_callback(void)
{
    xTaskCreate(TaskTemperature, "Temperature", 4096, NULL, 1, NULL);
}

void app_main(void)
{
    hl_wifi_init(&connected_callback);
}
