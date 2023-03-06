#include <stdio.h>
#include "esp_log.h"
#include "driver/uart.h"
#include <string.h>

#define BUF_SIZE (1024)
#define UART_NUM (UART_NUM_0)

static const char *TAG = "log";

void count(uint8_t num)
{
    printf("Counting down from %u\n", num);
    while (num > 0)
    {
        char buf[BUF_SIZE] = {0}; // transform to string
        snprintf(buf, sizeof(buf), "%u", num);

        uart_write_bytes(UART_NUM, (const char*)buf, strlen(buf));
        char* msg = "\n";
        uart_write_bytes(UART_NUM, (const char*)msg, strlen(msg));
        num -= 1;
    }
}

void app_main ()
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // Install UART driver with default configuration
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));

    // Set UART pins (using default pins for UART0)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Read data from UART
    while (1) {
        uint8_t buf[BUF_SIZE] = {0};
        int len = uart_read_bytes(UART_NUM, buf, BUF_SIZE-1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            buf[len] = '\0';
            ESP_LOGI(TAG, "Received %d bytes: %s", len, buf);

            uint8_t input = (uint8_t)atoi((char*)buf);
            count(input);
        }
    }
}