#ifndef ESP_CONTROLLER_SERVICE_H
#define ESP_CONTROLLER_SERVICE_H

#include "esp_device.h"
#include "esp_errors.h"

Error_Code esp_controller_init(void);
Error_Code esp_controller_get_devices(esp_device_t *devices, size_t *device_count);
Error_Code esp_controller_turn_pin_on(const char *port, int pin, esp_response_t *response);
Error_Code esp_controller_turn_pin_off(const char *port, int pin, esp_response_t *response);
Error_Code esp_controller_read_sensor(const char *port, int pin, const char *model, const char *sensor, dht_response_t *response);

#endif