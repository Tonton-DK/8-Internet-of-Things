#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "string.h"

#define TX_BUFFER_SIZE 10
QueueHandle_t tx_queue;
#define AVG_BUFFER_SIZE 10
QueueHandle_t avg_queue;
#define AVG_WINDOW_SIZE 7
QueueHandle_t thres_queue;
#define THRES_BUFFER_SIZE 10
#define THRESHOLD 100

typedef struct
{
    QueueHandle_t input;
    QueueHandle_t output;
} stage_interface_t;

stage_interface_t avg_pair;
stage_interface_t thres_pair;

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

void TaskAvg(void *pvParameters)
{
    stage_interface_t *pair = (stage_interface_t *)pvParameters;
    QueueHandle_t input_queue = pair->input;
    QueueHandle_t output_queue = pair->output;

    int buffer[AVG_WINDOW_SIZE];
    memset(buffer, 0, AVG_WINDOW_SIZE * sizeof(buffer[0]));
    uint8_t index = 0;
    int sum = 0;
    int new;
    int value;

    // TODO: consume first AVG_WINDOW_SIZE-1 messages
    while (true)
    {
        while (xQueueReceive(input_queue, &new, 10) != pdPASS);
        int old = buffer[index];
        buffer[index] = new;
        sum = sum - old;
        sum = sum + new;
        index = (index + 1) % AVG_WINDOW_SIZE;
        value = sum / AVG_WINDOW_SIZE;
        while (xQueueSendToBack(output_queue, &value, 10) != pdTRUE);
    }
}

void TaskThreshold(void *pvParameters)
{
    stage_interface_t *pair = (stage_interface_t *)pvParameters;
    QueueHandle_t input_queue = pair->input;
    QueueHandle_t output_queue = pair->output;

    int last = 1<<16;
    int current;
    while (true)
    {
        while (xQueueReceive(input_queue, &current, 10) != pdPASS);
        if (abs(current - last) > THRESHOLD)
        {
            while (xQueueSendToBack(output_queue, &current, 10) != pdTRUE);
            last = current;
        }
    }
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_11);

    tx_queue = xQueueCreate(TX_BUFFER_SIZE, sizeof(int));
    avg_queue = xQueueCreate(AVG_BUFFER_SIZE, sizeof(int));
    thres_queue = xQueueCreate(THRES_BUFFER_SIZE, sizeof(int));
    avg_pair = (stage_interface_t){tx_queue, avg_queue};
    thres_pair = (stage_interface_t){avg_queue, thres_queue};

    xTaskCreate(TaskSample, "Sample", 4096, tx_queue, 0, NULL);
    xTaskCreate(TaskAvg, "Avg", 4096, &avg_pair, 0, NULL);
    xTaskCreate(TaskThreshold, "Threshold", 4096, &thres_pair, 0, NULL);
    xTaskCreate(TaskTransmit, "Transmit", 4096, thres_queue, 0, NULL);
}
