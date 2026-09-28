#include <uci.h>

#include "ubus.h"

static Ubus_State g_ubus;

Error_Code initialize_ubus(void){

    g_ubus.ctx = ubus_connect(NULL);

    if(g_ubus.ctx == NULL)
        return ERR_UBUS_CONNECT;

    return OK;
}

Error_Code disconnect_ubus(void){

    if (g_ubus.ctx != NULL){
        ubus_free(g_ubus.ctx);
        g_ubus.ctx = NULL;
    }

    return OK;
}

Error_Code ubus_get_object_ids(void){
    
    int err;

    err = ubus_lookup_id(g_ubus.ctx, "system", &g_ubus.system_id);
    if(err != 0)
        return ERR_UBUS_SYSTEM_LOOKUP;

    err = ubus_lookup_id(g_ubus.ctx, "network.device", &g_ubus.network_device_id);
    if (err != 0)
        return ERR_UBUS_NETWORK_DEVICE_LOOKUP;

    return OK;
}

Ubus_State *get_ubus_state(void)
{
    return &g_ubus;
}