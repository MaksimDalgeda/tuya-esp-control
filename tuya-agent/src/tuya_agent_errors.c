#include "tuya_agent_errors.h"

Error parser_error_code(Error_Code code)
{
    switch (code)
    {
        case OK:
            return OK_T;

        case CPU_READ_ERROR:
        case CPU_INVALID_ERROR:
            return ERROR_CPU_T;

        case RAM_ERROR:
            return ERROR_RAM_T;

        case NETWORK_ERROR:
            return ERROR_NETWORK_T;

        case UPTIME_ERROR:
            return ERROR_UPTIME_T;

        case ERR_UBUS_CONNECT:
        case ERR_UBUS_INVOKE:
        case ERR_UBUS_NOT_INITIALIZED:
            return ERROR_UBUS_INITIALIZE_T;

        case ERR_UBUS_SYSTEM_LOOKUP:
        case ERR_UBUS_LAN_LOOKUP:
        case ERR_UBUS_WAN_LOOKUP:    
            return ERROR_UBUS_DATA_T;

        default:
            return ERROR_T;
    }
}

const char *error_to_string(Error err)
{
    switch (err)
    {
        case OK_T:
            return "OK_T";

        case ERROR_T:
            return "ERROR_T";

        case ERROR_CONNECT_T:
            return "ERROR_CONNECT_T";

        case ERROR_TUYA_AUTH_T:
            return "ERROR_TUYA_AUTH_T";

        case ERROR_TUYA_TIME_OUT_T:
            return "ERROR_TUYA_TIME_OUT_T";

        case ERROR_TUYA_SEND_T:
            return "ERROR_TUYA_SEND_T";

        case ERROR_CPU_T:
            return "ERROR_CPU_T";

        case ERROR_RAM_T:
            return "ERROR_RAM_T";

        case ERROR_NETWORK_T:
            return "ERROR_NETWORK_T";

        case ERROR_UPTIME_T:
            return "ERROR_UPTIME_T";

        case ERROR_PARSE_T:
            return "ERROR_PARSE_T";

        case ERROR_NULL_POINTER_T:
            return "ERROR_NULL_POINTER_T";

        case  ERROR_UBUS_INITIALIZE_T:
            return "ERROR_UBUS_INITIALIZE_T";

        case ERROR_UBUS_DATA_T:
            return "ERROR_UBUS_DATA_T";

        default:
            return "UNKNOWN_ERROR";
    }
}