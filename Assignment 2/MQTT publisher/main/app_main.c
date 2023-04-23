#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *TAG = "MQTT_PUBLISHER";

#define DEFAULT_VREF    3300.0  // Default ADC reference voltage in mV
#define LMT86_GAIN      -10.9   // Average sensor gain of the LMT86 temperature sensor
#define LMT86_OFFSET    2103.0  // Offset voltage of the LMT86 temperature sensor

// Replace these
#define ENDPOINT        "mqtt://192.168.0.152"
#define DEVICE_ID       "Temp_Sensor"

bool run;

typedef struct {
  esp_mqtt_client_handle_t client;
  uint32_t sample_rate;
  uint32_t batch_size;
  char topic[12];
  uint32_t current;
  uint32_t* batches;
  time_t* timestamps;
  char* csv_str;
} paramStruct;

uint32_t sample(adc1_channel_t channel)
{
    return adc1_get_raw(channel);
}

uint32_t convert(uint32_t raw)
{
    float voltage = (float)raw * DEFAULT_VREF / 4095.0;
	printf("Voltage: %.2f \n", voltage);
    return (voltage - LMT86_OFFSET) / LMT86_GAIN;
}

void insert(uint32_t value, paramStruct *params)
{
    params->batches[params->current] = value;
    params->timestamps[params->current] = time(NULL);
    params->current++;
}

void toCSV(paramStruct *params)
{
    for (int i = 0; i < params->batch_size; i++) {
        sprintf(params->csv_str + strlen(params->csv_str), "%llu#%lu,", params->timestamps[i], params->batches[i]);
    }
    params->csv_str[strlen(params->csv_str) - 1] = '\0';
    printf("The CSV string is: %s\n", params->csv_str);
}

void post(paramStruct *params, uint32_t value)
{
    char buffer[16];
	sprintf(buffer, "%ld\n", value);
    int msg_id = esp_mqtt_client_publish(params->client, "topic/temperature", buffer, 0, 1, 0);
    ESP_LOGI(TAG, "sent publish successful: msg_id=%d, msg=%s", msg_id, buffer);
}

void reset(paramStruct *params)
{
    params->current = 0;
    memset(params->batches, 0, params->batch_size * sizeof(uint32_t));
    memset(params->csv_str, 0, params->batch_size * 2 * sizeof(uint32_t));
}

static esp_adc_cal_characteristics_t adc1_chars;

void CollectTemps(void *pvParameters){
	
	paramStruct *params = (paramStruct*) pvParameters; 
	
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11));
    
	reset(params);
	while (run)
	{
        uint32_t sam = adc1_get_raw(ADC1_CHANNEL_5);
        printf("Raw: %ld \n", sam);
        uint32_t voltage = esp_adc_cal_raw_to_voltage(sam, &adc1_chars);
        printf("Voltage: %ld mV \n", voltage);
        uint32_t temp = (voltage - LMT86_OFFSET) / LMT86_GAIN;
        printf("Temperature: %ld C \n\n", temp);

		post(params, temp);
		
        //insert(temp, params);
		// Send CSV data
		//if (params->current >= params->batch_size) 
		//{ 
		//	toCSV(params);
		//	reset(params);
		//}
		
		vTaskDelay(pdMS_TO_TICKS(params->sample_rate));
	}
	
	vTaskDelete(NULL);
}

paramStruct params_TempSensor;
static void start_TempSensor(esp_mqtt_client_handle_t client){
	params_TempSensor = (paramStruct){ 
        .client=client, 
        .sample_rate=1000, 
        .batch_size=10, 
        .topic="Temperature", 
        .current=0};
    params_TempSensor.batches = malloc(params_TempSensor.batch_size * sizeof(uint32_t));
    params_TempSensor.timestamps = malloc(params_TempSensor.batch_size * sizeof(time_t));
    params_TempSensor.csv_str = malloc(params_TempSensor.batch_size * 2 * sizeof(uint32_t));
    xTaskCreate(CollectTemps, "CollectTemps_TempSensorCeiling", 4096, &params_TempSensor, 1, NULL);        
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            run = true;
            start_TempSensor(client);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            run = false;
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = ENDPOINT,
        .credentials.client_id = DEVICE_ID,
        .credentials.set_null_client_id = false
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
}
