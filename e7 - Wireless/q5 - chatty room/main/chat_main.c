#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hl_wifi.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <mqtt_client.h>

#include "esp_log.h"

#define BROKER_URL "mqtt://broker.hivemq.com"
#define TOPIC "org/sdu/course/iot/year/2022/chat/channel/42"

#define MAX_NAME_LENGTH 10
#define MAX_INPUT_LENGTH 256
#define MAX_MESSAGE_LENGTH 512

char nick[MAX_NAME_LENGTH];
char buffer[MAX_INPUT_LENGTH];
char message[MAX_MESSAGE_LENGTH];

esp_mqtt_client_handle_t client;

void TaskChat(void *pvParameters)
{
    while (true)
    {
        while (strlen(buffer) <= 0)
        {
            scanf("%s", buffer);
        }
        sprintf(message, "%s: %s\n", nick, buffer);
        esp_mqtt_client_publish(client, TOPIC, message, 0, 1, 0);
        memset(buffer, 0, sizeof(buffer));
    }
}

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    printf("Receiving hits!\n");
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;

    switch (((esp_mqtt_event_id_t)event_id))
    {
    case MQTT_EVENT_CONNECTED:
        esp_mqtt_client_subscribe(client, TOPIC, 1);
        xTaskCreate(TaskChat, "Chat", 4096, NULL, 1, NULL);
        break;
    case MQTT_EVENT_DATA:
        // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    default:
        printf("Event: %ld\n", event_id);
    }
}

void connected_callback(void)
{
    // Prompt the user for a nickname
    printf("Please enter your nickname (up to 10 characters): ");

    while (strlen(nick) <= 0)
    {
        scanf("%s", nick);
    }

    // Print the nickname for confirmation
    printf("Your nickname is %s\n", nick);

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URL,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client,
                                   ESP_EVENT_ANY_ID,
                                   mqtt_event_handler,
                                   NULL);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_DEBUG);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    hl_wifi_init(&connected_callback);
}
