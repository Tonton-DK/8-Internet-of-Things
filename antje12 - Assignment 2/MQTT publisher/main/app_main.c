#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "hl_wifi.h"

// Dynamic config
#define PERIOD CONFIG_PERIOD
#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD
#define BROKER CONFIG_MQTT_BROKER
#define TOPIC CONFIG_MQTT_TOPIC

// Static config
#define CHANNEL ADC1_CHANNEL_6
#define UNIT ADC_UNIT_1

#define LMT86_GAIN -10.9    // Average sensor gain of the LMT86 temperature sensor
#define LMT86_OFFSET 2103.0 // Offset voltage of the LMT86 temperature sensor

static const char *TAG = "MQTT_TEMP_PUBLISHER";

bool run;
static esp_adc_cal_characteristics_t adc_chars;

void init_adc()
{
    esp_adc_cal_characterize(UNIT, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(CHANNEL, ADC_ATTEN_DB_11));
}

uint32_t collectTemp()
{
    int sam = adc1_get_raw(CHANNEL);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(sam, &adc_chars);
    return (voltage - LMT86_OFFSET) / LMT86_GAIN;
}

void post(esp_mqtt_client_handle_t client, uint32_t value)
{
    char buffer[4];
    sprintf(buffer, "%ld", value);
    int msg_id = esp_mqtt_client_publish(client, TOPIC, buffer, 0, 1, 0);
    ESP_LOGI(TAG, "Successful publish: msg_id=%d, msg=%s", msg_id, buffer);
}

void CollectTemps(void *pvParameters)
{
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;
    init_adc();

    while (run)
    {
        uint32_t temp = collectTemp();
        printf("Temperature: %ld C \n", temp);
        post(client, temp);
        vTaskDelay(pdMS_TO_TICKS(PERIOD));
    }

    vTaskDelete(NULL);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        run = true;
        xTaskCreate(CollectTemps, "CollectTemps", 4096, client, 1, NULL);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        run = false;
        break;
    default:
        // ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    hl_wifi_init(&mqtt_app_start);
}
