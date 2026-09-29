#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>
#include <unistd.h>

#include "signal_handler.h"
#include "daemon.h"
#include "cli_handler.h"
#include "tuya_agent.h"
#include "tuya_agent_errors.h"
#include "ubus.h"

int main(int argc, char *argv[])
{
    Error err = OK_T;
    Parameters parameters;

    openlog("tuya-monitor-daemon", LOG_PID | LOG_CONS, LOG_DAEMON);

    set_signal_action();

    syslog(LOG_INFO, "Application started");

    err = parse_args(argc, argv, &parameters);

    if (err != OK_T)
        goto end;

    if (parameters.daemon) {
        err = daemonize();
        if (err != OK_T)
            goto end;
    }

    err = tuya_agent_init(parameters.device_id, parameters.device_secret);

    if (err != OK_T)
        goto end;

    syslog(LOG_INFO, "Tuya initialization successful");

    err = tuya_agent_connect();

    if (err != OK_T)
        goto end;

    syslog(LOG_INFO, "Connected to Tuya cloud");

    err = parser_error_code(
        initialize_ubus());

    if (err != OK_T)
        goto end;

    err = parser_error_code(
        ubus_get_object_ids());

    if (err != OK_T)
        goto end;

    syslog(LOG_INFO, "Connected to UBUS");

    while (!stop) {
        tuya_agent_loop();
        usleep(100000);
    }

end:

    disconnect_ubus();
    tuya_agent_deinit();
    if (err == OK_T) {
        syslog(LOG_INFO,"Application stopped without error");}
    else 
        syslog(LOG_ERR, "Application stopped with code %s (%d)", error_to_string(err), err);
    closelog();

    return err;
}