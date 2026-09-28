#ifndef CLI_HANDLER_H
#define CLI_HANDLER_H

#include <stdbool.h>

#include "tuya_agent_errors.h"

typedef struct
{
    char *device_id;
    char *device_secret;
    char *product_id;
    bool daemon;

} Parameters;

Error parse_args(int argc, char *argv[], Parameters * parameters);

#endif