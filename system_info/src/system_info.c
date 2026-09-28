#include <stdint.h>
#include <signal.h>

#include "internal.h"
#include "ubus_internal.h"

Error_Code get_system_info(system_info_t *info){
    
    Error_Code err;

    err = get_cpu_usage(&info->cpu_usage_prcnt); 
    if(err > OK)
        return err;

    err = ubus_get_system_network(info->network, &info->network_count);
    if(err > OK)
        return err;

    err = ubus_get_system_traffic(info->network, &info->network_count);
    if(err > OK)
        return err;

    err = ubus_get_system_info(info);
    if(err > OK)
        return err;

    return OK;
}