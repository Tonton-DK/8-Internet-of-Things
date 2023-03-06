#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "esp_log.h"

#define BUF_SIZE (1024)
#define UART_NUM (UART_NUM_0)

static const char *TAG = "log";

void initUart()
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
}

typedef void (*command_t)(int argc, char* argv[]);

typedef struct dispatch_entry {
    char* name;
    command_t command;
} dispatch_entry_t;

#define COMMAND_COUNT (1)

void command_echo(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        //printf("argv[%d]: %s\n", i, argv[i]);
        uart_write_bytes(UART_NUM, (const char*)argv[i], strlen(argv[i]));
        char* msg = "\n";
        uart_write_bytes(UART_NUM, (const char*)msg, strlen(msg));
    }
}

dispatch_entry_t command_dispatch_table[] = {
    {"echo", command_echo}
};

void dispatch(char* string)
{
    int result = 0;
    char* token = strtok(string, " ");
    for (int i = 0; i < sizeof(command_dispatch_table) / sizeof(dispatch_entry_t); i++) {
        if (strcmp(command_dispatch_table[i].name, string) == 0) {
            int count = 0;
            char* argv[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
            while (token != NULL) {
                argv[count] = token;
                count++;
                token = strtok(NULL, " ");
            }
            command_echo(count, argv);
            result = 1;
        }
    }

    if (result == 0){
        char* msg = "Could not process command...\n";
        uart_write_bytes(UART_NUM, (const char*)msg, strlen(msg));
    }
}

void app_main(void)
{
    initUart();

    // Read data from UART
    while (1) {
        uint8_t buf[BUF_SIZE] = {0};
        int len = uart_read_bytes(UART_NUM, buf, BUF_SIZE-1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            buf[len] = '\0';
            ESP_LOGI(TAG, "Received %d bytes: %s", len, buf);
            dispatch((char*)buf);
        }
    }
}