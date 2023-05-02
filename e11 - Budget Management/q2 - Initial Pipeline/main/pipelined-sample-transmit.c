#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define TX_BUFFER_SIZE 10
QueueHandle_t tx_queue;

void TaskSample(void *pvParameters)
{
    QueueHandle_t output_queue = (QueueHandle_t)pvParameters;
    int value;
    while (true)
    {
        value = adc1_get_raw(ADC_CHANNEL_6);
        while (xQueueSendToBack(output_queue, &value, 10) != pdTRUE);
    }
    vTaskDelete(NULL);
}

void TaskTransmit(void *pvParameters)
{
    QueueHandle_t input_queue = (QueueHandle_t)pvParameters;
    int value;
    while (true)
    {
        while (xQueueReceive(input_queue, &value, 10) != pdPASS);
        printf("%u\n", value);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_11);

    tx_queue = xQueueCreate(TX_BUFFER_SIZE, sizeof(int));
    xTaskCreate(TaskSample, "Sample", 4096, tx_queue, 1, NULL);
    xTaskCreate(TaskTransmit, "Transmit", 4096, tx_queue, 1, NULL);
}
