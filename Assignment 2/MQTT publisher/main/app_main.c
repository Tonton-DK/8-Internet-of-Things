#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "protocol_examples_common.h"

#define CHANNEL CONFIG_ADC_CHANNEL
#define UNIT CONFIG_ADC_UNIT
#define FREQUENCY CONFIG_FREQUENCY
#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD
#define BROKER CONFIG_MQTT_BROKER
#define TOPIC CONFIG_MQTT_TOPIC
#define DEVICE_ID "Temp_Sensor"
#define LMT86_GAIN -10.9    // Average sensor gain of the LMT86 temperature sensor
#define LMT86_OFFSET 2103.0 // Offset voltage of the LMT86 temperature sensor

static const char *TAG = "MQTT_TEMP_PUBLISHER";

bool run;
static esp_adc_cal_characteristics_t adc1_chars;

uint32_t collectTemp()
{
    uint32_t sam = adc1_get_raw(ADC1_CHANNEL_5);
    //printf("Raw: %ld \n", sam);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(sam, &adc1_chars);
    //printf("Voltage: %ld mV \n", voltage);
    uint32_t temp = (voltage - LMT86_OFFSET) / LMT86_GAIN;
    printf("Temperature: %ld C \n", temp);
    return temp;
}

void post(esp_mqtt_client_handle_t client, uint32_t value)
{
    char buffer[4];
    sprintf(buffer, "%ld", value);
    int msg_id = esp_mqtt_client_publish(client, TOPIC, buffer, 0, 1, 0);
    ESP_LOGI(TAG, "sent publish successful: msg_id=%d, msg=%s", msg_id, buffer);
}

void CollectTemps(void *pvParameters){
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t) pvParameters;

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11));

    while (run)
    {
        uint32_t temp = collectTemp();
        post(client, temp);
        vTaskDelay(pdMS_TO_TICKS(FREQUENCY * 1000));
    }

    vTaskDelete(NULL);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
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
        //ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER,
        .credentials.client_id = DEVICE_ID,
        .credentials.set_null_client_id = false};

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
