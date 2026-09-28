#ifndef TUYA_AGENT_ERRORS_H
#define TUYA_AGENT_ERRORS_H

#include "system_info_errors.h"

typedef enum
{
    OK_T,
    ERROR_T,
    ERROR_NULL_POINTER_T,
    ERROR_CONNECT_T,
    ERROR_TUYA_AUTH_T,
    ERROR_TUYA_TIME_OUT_T,
    ERROR_TUYA_SEND_T,
    ERROR_CPU_T,
    ERROR_RAM_T,
    ERROR_NETWORK_T,
    ERROR_UPTIME_T,
    ERROR_PARSE_T,
    ERROR_UBUS_INITIALIZE_T,
    ERROR_UBUS_DATA_T

} Error;

Error parser_error_code(Error_Code error);
Error initialize_ubus_connection();
const char *error_to_string(Error err);

#endif