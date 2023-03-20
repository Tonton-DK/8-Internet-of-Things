#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hl_wifi.h"

void connected_callback(void)
{
    printf("Callback reached!\n");
}

void app_main(void)
{
    hl_wifi_init(&connected_callback);
}
