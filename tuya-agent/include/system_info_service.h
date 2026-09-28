#ifndef SYSTEM_INFO_SERVICE_H
#define SYSTEM_INFO_SERVICE_H

#define MAX_INTERFACES_TUYA 10

#include <stddef.h>
#include "tuya_agent_errors.h"
#include "system_info.h"

typedef struct
{
    char name[32];
    char ip[16];
    char netmask[16];
    double rx_mb;
    double tx_mb;

} tuya_network_info_t;

typedef struct
{
    double total_ram_mb;
    double free_ram_mb;
    double cpu_usage_prcnt;
    long uptime_s;
    tuya_network_info_t network[MAX_INTERFACES_TUYA];
    size_t network_count;

} tuya_system_info_t;

Error system_info_service(system_info_t *info);
void initialize_ubus_disconnect();
Error initialize_ubus_connection();

#endif