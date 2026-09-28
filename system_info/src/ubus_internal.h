#ifndef UBUS_INTERNAL_H
#define UBUS_INTERNAL_H

#include "ubus.h"

Error_Code ubus_get_system_info(system_info_t *info);
Error_Code get_network_interfaces(network_info_t *network, size_t *network_count);
Error_Code ubus_get_system_network(network_info_t *network, size_t *network_count);
Error_Code ubus_get_system_traffic(network_info_t *network, size_t *network_count);

#endif