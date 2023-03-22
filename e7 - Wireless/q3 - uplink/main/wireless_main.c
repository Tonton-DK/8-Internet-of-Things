#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hl_wifi.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void TaskCount(void *pvParameters)
{
    printf("Callback reached!\n");
    char* ip = "192.168.1.102";
    uint16_t host = 8000;
    sockaddr_in_t addr = hl_wifi_make_addr(ip, host);
    int con = hl_wifi_tcp_connect(addr);
    if (con == -1)
    {
        printf("Connection failure\n");
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
