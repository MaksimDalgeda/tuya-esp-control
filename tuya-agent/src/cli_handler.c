#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "cli_handler.h"

static const struct option long_options[] = {
    {"device-id",     required_argument, 0, 'd'},
    {"device-secret", required_argument, 0, 's'},
    {"product-id",    required_argument, 0, 'p'},
    {"daemon",        no_argument,       0, 'D'},
    {0, 0, 0, 0}
};

static bool is_empty(const char *argument)
{
    while (*argument) {
        if (!isspace((unsigned char)*argument))
            return false;

        argument++;
    }
    return true;
}

Error parse_args(int argc, char *argv[], Parameters *parameters)
{
    int option;

    memset(parameters, 0, sizeof(*parameters));

    opterr = 0;
    optind = 1;

    while ((option = getopt_long(argc,argv, "d:s:p:D", long_options, NULL)) != -1)
    {
        switch (option) {

        case 'd':
            parameters->device_id = optarg;
            break;

        case 's':
            parameters->device_secret = optarg;
            break;

        case 'p':
            parameters->product_id = optarg;
            break;

        case 'D':
            parameters->daemon = true;
            break;

        case '?':
        default:
            return ERROR_PARSE_T;
        }
    }

    if (!parameters->device_id)
        return ERROR_PARSE_T;

    if (!parameters->device_secret)
        return ERROR_PARSE_T;

    if (!parameters->product_id)
        return ERROR_PARSE_T;

    if (is_empty(parameters->device_id) || is_empty(parameters->device_secret) || is_empty(parameters->product_id)){
        return ERROR_PARSE_T;
    }

    return OK_T;
}