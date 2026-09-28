#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#define MAX_INTERFACES 10

#include <stdint.h>
#include <stddef.h>

#include "system_info_errors.h"

typedef struct
{
    char name[32];
    char ip[16];
    char netmask[16];
    uint64_t rx_bytes;
    uint64_t tx_bytes;

} network_info_t;

typedef struct
{
    long total_ram_bytes;
    long free_ram_bytes;
    double cpu_usage_prcnt;
    long uptime_s;
    network_info_t network[MAX_INTERFACES];
    size_t network_count;

} system_info_t;

Error_Code get_system_info(system_info_t *info);

#endif