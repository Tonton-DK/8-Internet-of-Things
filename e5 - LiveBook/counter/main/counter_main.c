/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#define DELAY (200 / portTICK_PERIOD_MS)

void app_main(void)
{
    uint8_t counter = 0;
    while(1){
        printf("%d\n", counter);
        counter++;
        vTaskDelay(DELAY);
    }
}