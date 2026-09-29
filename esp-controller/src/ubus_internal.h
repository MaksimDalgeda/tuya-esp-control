#ifndef UBUS_INTERNAL_H
#define UBUS_INTERNAL_H

#include "esp_errors.h"
#include "esp_device.h"

Error_Code ubus_get_devices(esp_device_t *devices, size_t *device_count);
Error_Code ubus_pin_on(const char *port, int pin);
Error_Code ubus_pin_off(const char *port, int pin);
Error_Code ubus_get_sensor(const char *port, int pin, const char *model, const char *sensor);

#endif