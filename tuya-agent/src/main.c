#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>
#include <time.h>

#include "signal_handler.h"
#include "daemon.h"
#include "cli_handler.h"


int main(int argc, char *argv[])
{   
    Error err;
    Parameters parameters;
    openlog("tuya-monitor-daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
    set_signal_action();

    err = parse_args(argc, argv, &parameters);

    syslog(LOG_INFO, "Application started");

    if(err != OK_T)
        goto end;

    if(parameters.daemon){
        err = daemonize();
        if(err != OK_T)
            goto end;
    }

    system_info_t info;
    tuya_system_info_t message;

    err = tuya_agent_init(parameters.device_id, parameters.device_secret);
    if(err != OK_T)
        goto end;
    syslog(LOG_INFO, "Connect request successful");

    err = tuya_agent_connect();
    if(err != OK_T)
        goto end;

    syslog(LOG_INFO, "Connected to Tuya cloud");


    err = initialize_ubus_connection();
    if(err != OK_T)
        goto end;

    time_t last_send = 0;

    while(!stop)
    {
        tuya_agent_loop();

        usleep(100000);
    }

    end:
    tuya_agent_deinit();
    initialize_ubus_disconnect();
    if(err == OK_T)
        syslog(LOG_INFO, "Application stopped without error");
    else
        syslog(LOG_INFO, "Application stopped with code - %s (%d)",error_to_string(err), err);
    closelog();
    return err;
}