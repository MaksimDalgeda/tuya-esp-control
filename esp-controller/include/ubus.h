#ifndef UBUS_H
#define UBUS_H

#include <libubus.h>
#include <syslog.h>

#include "esp_errors.h"

typedef struct
{
    struct ubus_context *ctx;
    uint32_t esp_controller_id;

} Ubus_State; 


Error_Code initialize_ubus(void);
Error_Code ubus_get_object_ids(void);
Error_Code disconnect_ubus(void);

Ubus_State *get_ubus_state(void);

#endif