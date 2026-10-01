#include <syslog.h>

#include "esp_controller_service.h"
#include "ubus.h"

Error_Code esp_controller_init(void)
{
    Error_Code err;

    syslog(LOG_INFO, "Initializing ESP controller service");

    err = initialize_ubus();

    if (err != OK)
        return err;

    err = ubus_get_object_ids();

    if (err != OK)
        return err;

    syslog(LOG_INFO, "ESP controller service initialized");

    return OK;
}

Error_Code esp_controller_get_devices( esp_device_t *devices,size_t *device_count)
{   
    syslog(LOG_INFO, "Getting ESP devices");
    Error_Code err;

    err = ubus_get_devices(devices, device_count);

    if(err != OK){
        syslog(LOG_ERR, "Failed to get ESP devices");
        return err;
    }

    return OK;
}

Error_Code esp_controller_turn_pin_on(const char *port, int pin, esp_response_t *response)
{
    syslog(LOG_INFO,"Turning pin ON: port=%s pin=%d", port, pin);
    Error_Code err;

    err = ubus_pin_on(port, pin, response);

    if(err != OK){
        syslog(LOG_ERR, "Failed to turn pin ON");
        return err;
    }

    return OK;
}

Error_Code esp_controller_turn_pin_off(const char *port, int pin, esp_response_t *response)
{
    syslog(LOG_INFO,"Turning pin OFF: port=%s pin=%d", port, pin);
    Error_Code err;

    err = ubus_pin_off(port, pin, response);

    if(err != OK){
        syslog(LOG_ERR, "Failed to turn pin OFF");
        return err;
    }

    return OK;
}

Error_Code esp_controller_read_sensor(const char *port, int pin, const char *model, const char *sensor,dht_response_t *response)
{
    syslog(LOG_INFO,"Reading sensor: port=%s pin=%d model=%s sensor=%s", port, pin, model, sensor);

    Error_Code err;

    err = ubus_get_sensor(port, pin, model, sensor, response);

    if(err != OK){
        syslog(LOG_ERR, "Failed to read sensor");
        return err;
    }
    return OK;
}