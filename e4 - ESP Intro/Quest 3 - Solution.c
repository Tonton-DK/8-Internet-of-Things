#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define DELAY(ms) ((ms) / portTICK_PERIOD_MS)
#define BUFSIZE (20)
#define MAX_READ_DELAY (20/portTICK_RATE_MS)
#define UART (UART_NUM_0)

uint8_t buffer[BUFSIZE];

int readint ()
{
    uint8_t i = 0;     // total number of bytes read
    uint8_t value = 0; // current "accumulated" value
    uint8_t size;      // number of bytes read
    
    while (i<BUFSIZE) {
        size = uart_read_bytes(UART, buffer, BUFSIZE-i, MAX_READ_DELAY);
        for (uint8_t j=0 ; j<size ; j++) {
            uint8_t digit = buffer[j];
            
            // guard: end of line
            if (digit=='\n' || digit=='\r') return value;
            
            value = value*10+(digit-'0');
        }
        i += size;
    }
    
    printf("Error reading uint8_t. Falling back to a reasonable default ...\n");
    return 42;
}

void TaskCountdown (void *pvParameters)
{
    while (1) {
        printf("Please enter an integer (in range [0-255]) ...\n");
        uint8_t value = readint();
        
        do {
            printf("%d\n", value);
            vTaskDelay(DELAY(100));
        } while (value--!=0);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(uart_driver_install(UART, UART_FIFO_LEN*2, 0, 0, NULL, 0));
    
    xTaskCreate(TaskCountdown, "Countdown", 4096, NULL, 1, NULL);
}
