#ifndef ESP_DEVICE_H
#define ESP_DEVICE_H

#define MAX_ESP_DEVICES 10

typedef struct
{
    char port[64];
    char vendor_id[16];
    char product_id[16];

} esp_device_t;

#endif