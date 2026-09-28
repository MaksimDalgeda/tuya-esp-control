#include <string.h>
#include "system_info_convertor.h"


Error convert_system_info(system_info_t *info, tuya_system_info_t *message)
{    
    if (info == NULL || message == NULL){
        return ERROR_NULL_POINTER_T;
    }
    
    message->total_ram_mb=convert_ram_to_mb(info->total_ram_bytes);
    message->free_ram_mb=convert_ram_to_mb(info->free_ram_bytes);
    message->cpu_usage_prcnt=info->cpu_usage_prcnt;
    message->uptime_s=info->uptime_s;
    message->network_count =(info->network_count > MAX_INTERFACES_TUYA)? MAX_INTERFACES_TUYA: info->network_count;

    for(size_t i = 0; i < message->network_count; i++) {

        strncpy(message->network[i].name, info->network[i].name,  sizeof(message->network[i].name) - 1);
        message->network[i].name[sizeof(message->network[i].name) - 1] = '\0';

        strncpy(message->network[i].ip, info->network[i].ip, sizeof(message->network[i].ip) - 1);
        message->network[i].ip[sizeof(message->network[i].ip) - 1] = '\0';

        strncpy(message->network[i].netmask, info->network[i].netmask, sizeof(message->network[i].netmask) - 1);
        message->network[i].netmask[sizeof(message->network[i].netmask) - 1] = '\0';

        message->network[i].rx_mb = convert_bytes_to_mb(info->network[i].rx_bytes);

        message->network[i].tx_mb = convert_bytes_to_mb(info->network[i].tx_bytes);
    }

    return OK_T;
}

double convert_ram_to_mb(long data)
{
    return (double)data / (1024.0 * 1024.0);
}

double convert_bytes_to_mb(uint64_t data)
{
    return (double)data / (1024.0 * 1024.0);
}