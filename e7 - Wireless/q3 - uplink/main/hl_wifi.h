#ifndef __HL_WIFI_H
#define __HL_WIFI_H

#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr_in sockaddr_in_t;
sockaddr_in_t hl_wifi_make_addr(char* ip, uint16_t host);

typedef void (*connect_callback_t)(void);
void hl_wifi_init(connect_callback_t callback);

int hl_wifi_tcp_connect(sockaddr_in_t addr);

#endif
