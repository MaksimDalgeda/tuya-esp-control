#ifndef ESP_DEVICE_H
#define ESP_DEVICE_H

#include <stddef.h>

#define MAX_ESP_DEVICES 10

typedef struct
{
    char port[64];
    char vendor_id[32];
    char product_id[32];

} esp_device_t;

typedef struct
{
    int rc;
    char msg[128];

    int error_code;
    char error_message[128];

} esp_response_t;

typedef struct
{
    int rc;

    char msg[128];

    float humidity;
    float temperature;

    int error_code;
    char error_message[128];

} dht_response_t;

#endif