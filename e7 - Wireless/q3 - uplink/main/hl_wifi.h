#ifndef __HL_WIFI_H
#define __HL_WIFI_H

typedef void (*connect_callback_t)(void);

void hl_wifi_init(connect_callback_t callback);
#endif
