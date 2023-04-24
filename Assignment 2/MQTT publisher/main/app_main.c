#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "hl_wifi.h"

#define CHANNEL CONFIG_ADC_CHANNEL
#define UNIT CONFIG_ADC_UNIT

#define PERIOD CONFIG_PERIOD
#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD
#define BROKER CONFIG_MQTT_BROKER
#define TOPIC CONFIG_MQTT_TOPIC

#define DEVICE_ID "Temp_Sensor"
#define LMT86_GAIN -10.9    // Average sensor gain of the LMT86 temperature sensor
#define LMT86_OFFSET 2103.0 // Offset voltage of the LMT86 temperature sensor

static const char *TAG = "MQTT_TEMP_PUBLISHER";

bool run;
static esp_adc_cal_characteristics_t adc_chars;
adc_unit_t unit;
adc1_channel_t adc1_channel;
adc2_channel_t adc2_channel;

// String to Enum struct
struct UnitEnum
{
    const char *str;
    adc_unit_t value;
};

// String to Enum map
static const struct UnitEnum unitEnumMap[] = {
    {"ADC_UNIT_1", ADC_UNIT_1},
    {"ADC_UNIT_2", ADC_UNIT_2}};

// String to Enum mapper
adc_unit_t get_unit()
{
    for (int i = 0; unitEnumMap[i].str != NULL; i++)
    {
        if (strcmp(UNIT, unitEnumMap[i].str) == 0)
        {
            return unitEnumMap[i].value;
        }
    }
    return ADC_UNIT_1;
}

struct Channel1Enum
{
    const char *str;
    adc1_channel_t value;
};

static const struct Channel1Enum channel1EnumMap[] = {
    {"ADC_CHANNEL_0", ADC1_CHANNEL_0},
    {"ADC_CHANNEL_1", ADC1_CHANNEL_1},
    {"ADC_CHANNEL_2", ADC1_CHANNEL_2},
    {"ADC_CHANNEL_3", ADC1_CHANNEL_3},
    {"ADC_CHANNEL_4", ADC1_CHANNEL_4},
    {"ADC_CHANNEL_5", ADC1_CHANNEL_5},
    {"ADC_CHANNEL_6", ADC1_CHANNEL_6},
    {"ADC_CHANNEL_7", ADC1_CHANNEL_7}};

adc1_channel_t get_adc1_channel()
{
    for (int i = 0; channel1EnumMap[i].str != NULL; i++)
    {
        if (strcmp(CHANNEL, channel1EnumMap[i].str) == 0)
        {
            return channel1EnumMap[i].value;
        }
    }
    return ADC1_CHANNEL_0;
}

struct Channel2Enum
{
    const char *str;
    adc2_channel_t value;
};

static const struct Channel2Enum channel2EnumMap[] = {
    {"ADC_CHANNEL_0", ADC2_CHANNEL_0},
    {"ADC_CHANNEL_1", ADC2_CHANNEL_1},
    {"ADC_CHANNEL_2", ADC2_CHANNEL_2},
    {"ADC_CHANNEL_3", ADC2_CHANNEL_3},
    {"ADC_CHANNEL_4", ADC2_CHANNEL_4},
    {"ADC_CHANNEL_5", ADC2_CHANNEL_5},
    {"ADC_CHANNEL_6", ADC2_CHANNEL_6},
    {"ADC_CHANNEL_7", ADC2_CHANNEL_7},
    {"ADC_CHANNEL_8", ADC2_CHANNEL_8},
    {"ADC_CHANNEL_9", ADC2_CHANNEL_9}};

adc2_channel_t get_adc2_channel()
{
    for (int i = 0; channel2EnumMap[i].str != NULL; i++)
    {
        if (strcmp(CHANNEL, channel2EnumMap[i].str) == 0)
        {
            return channel2EnumMap[i].value;
        }
    }
    return ADC2_CHANNEL_0;
}

void init_adc()
{
    unit = get_unit();
    esp_adc_cal_characterize(unit, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc_chars);

    if (unit == ADC_UNIT_1)
    {
        adc1_channel = get_adc1_channel();
        ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
        ESP_ERROR_CHECK(adc1_config_channel_atten(adc1_channel, ADC_ATTEN_DB_11));
    }
    else if (unit == ADC_UNIT_2)
    {
        adc2_channel = get_adc2_channel();
        ESP_ERROR_CHECK(adc2_config_channel_atten(adc2_channel, ADC_ATTEN_DB_11));
    }
}

uint32_t collectTemp()
{
    int sam;
    if (unit == ADC_UNIT_1)
    {
        sam = adc1_get_raw(adc1_channel);
    }
    else if (unit == ADC_UNIT_2)
    {
        adc2_get_raw(adc2_channel, ADC_WIDTH_BIT_DEFAULT, &sam);
    }
    // printf("Raw: %ld \n", sam);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(sam, &adc_chars);
    // printf("Voltage: %ld mV \n", voltage);
    uint32_t temp = (voltage - LMT86_OFFSET) / LMT86_GAIN;
    printf("Temperature: %ld C \n", temp);
    return temp;
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
        post(client, temp);
        vTaskDelay(pdMS_TO_TICKS(PERIOD * 1000));
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
        //ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER,
        //.credentials.client_id = DEVICE_ID,
        //.credentials.set_null_client_id = false
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    hl_wifi_init(&mqtt_app_start);
}
