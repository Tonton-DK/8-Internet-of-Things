#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/adc.h"

#define DELAY(ms) ((ms) / portTICK_PERIOD_MS)
#define BUFSIZE (40)
#define MAX_READ_DELAY (20 / portTICK_PERIOD_MS)
#define UART (UART_NUM_0)

uint8_t intBuffer[BUFSIZE];
char charBuffer[BUFSIZE];

int readint ()
{
    uint8_t i = 0;      // total number of bytes read
    uint16_t value = 0; // current "accumulated" value
    uint8_t size;       // number of bytes read
    
    while (i<BUFSIZE) {
        size = uart_read_bytes(UART, intBuffer, BUFSIZE-i, MAX_READ_DELAY);
        for (uint8_t j=0 ; j<size ; j++) {
            uint8_t digit = intBuffer[j];
            
            // guard: end of line
            if (digit=='\n' || digit=='\r') return value;
            
            value = value*10+(digit-'0');
        }
        i += size;
    }
    
    printf("Error reading uint8_t. Falling back to a reasonable default ...\n");
    return 42;
}

char* readstring ()
{
    uint8_t i = 0;     // total number of bytes read
    uint8_t size;      // number of bytes read
    
    while (i < BUFSIZE) {
        size = uart_read_bytes(UART, (uint8_t*)charBuffer + i, BUFSIZE - i, MAX_READ_DELAY);
        for (uint8_t j = 0 ; j < size ; j++) {
            char digit = charBuffer[i + j];
            
            // guard: end of line
            if (digit == '\n' || digit == '\r') {
                charBuffer[i + j] = '\0'; // terminate string
                return charBuffer;
            }
        }
        i += size;
    }

    printf("Error reading string. Falling back to a reasonable default ...\n");
    return "default";
}

uint32_t sample(adc1_channel_t channel)
{
    return adc1_get_raw(channel);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);

    ESP_ERROR_CHECK(uart_driver_install(UART, UART_FIFO_LEN*2, 0, 0, NULL, 0));
    
    while (1) {
        printf("Shall we play a game?\n");
        char* response = readstring();
        printf("Your response: %s\n", response);

        if (strcmp(response, "y") == 0) {
            
        } else {
            printf("Restarting...\n");
            esp_restart();
        }
        
        uint32_t adc_reading = sample(ADC1_CHANNEL_5);
        printf("ADC reading: %ld\n", adc_reading);

        printf("I am thinking about a number between 0 and 4095 (both included). I bet you can’t guess it in 8 tries!\n");
        
        bool done = false;
        uint8_t count = 0;
        
        while(!done){
            printf("You have guessed %d times. Please try again.\n", count);
            uint16_t value = readint();
            printf("Your value: %d\n", value);

            count+=1;

            if (value == adc_reading){
                printf("You got it in %d tries! The value was: %ld\n", count, adc_reading);
                done = true;
            }
            else if (value < adc_reading){
                printf("You have to go higher\n");
            }
            else{
                printf("You have to go lower\n");
            }
        }
    }
}
