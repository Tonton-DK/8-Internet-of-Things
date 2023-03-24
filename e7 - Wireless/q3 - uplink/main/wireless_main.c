#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hl_wifi.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "string.h"

void TaskCount(void *pvParameters)
{
    printf("Callback reached!\n");
    char *ip = "192.168.1.104";
    uint16_t host = 8000;
    sockaddr_in_t addr = hl_wifi_make_addr(ip, host);
    int sock = hl_wifi_tcp_connect(addr);
    if (sock == -1)
    {
        printf("Connection failure\n");
        vTaskDelete(NULL);
    }

    //char* msg = "Hello, World\n";
    //uint16_t length = strlen(msg);
    //hl_wifi_tcp_tx(sock, msg, length);

    uint16_t counter = 1;
    char buffer[7];
    while (true) 
    {
        // write the number to the buffer
        sprintf(buffer, "%u\n", counter);
        uint16_t length = strlen(buffer);
        hl_wifi_tcp_tx(sock, buffer, length);
        counter+=1;
        // introduce a delay of 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void connected_callback(void)
{
    xTaskCreate(TaskCount, "Count", 4096, NULL, 1, NULL);
}

void app_main(void)
{
    hl_wifi_init(&connected_callback);
}
