#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>
#include <unistd.h>

#include "signal_handler.h"
#include "daemon.h"
#include "cli_handler.h"
#include "tuya_agent.h"
#include "tuya_agent_errors.h"

#include "esp_controller_service.h"

int main(int argc, char *argv[])
{
    Error err = OK_T;
    Parameters parameters;

    openlog("tuya-monitor-daemon", LOG_PID | LOG_CONS,LOG_DAEMON);

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

    err = parser_error_code(esp_controller_init());

    if (err != OK_T)
        goto end;

    syslog(LOG_INFO, "Connected to ESP Controller");


    /* TEST BLOCK

    esp_device_t devices[10];
    size_t count = 0;

    Error_Code rc;

    rc = esp_controller_get_devices(
            devices,
            &count);

    printf("\n");
    printf("Get devices rc=%d\n", rc);

    for (size_t i = 0; i < count; i++) {

        printf(
            "port=%s vendor=%s product=%s\n",
            devices[i].port,
            devices[i].vendor_id,
            devices[i].product_id);
    }

    esp_response_t response;

    rc = esp_controller_turn_pin_on(
            "/dev/ttyUSB0",
            5,
            &response);

    printf(
        "pin_on rc=%d msg=%s err=%d err_msg=%s\n",
        response.rc,
        response.msg,
        response.error_code,
        response.error_message);

    sleep(5);

    rc = esp_controller_turn_pin_off(
        "/dev/ttyUSB0",
        5,
        &response);

    sleep(5);
    dht_response_t sensor;

    rc = esp_controller_read_sensor(
            "/dev/ttyUSB0",
            14,
            "dht11",
            "dht",
            &sensor);

    printf(
        "rc=%d\n"
        "msg=%s\n"
        "humidity=%.1f\n"
        "temperature=%.1f\n",
        sensor.rc,
        sensor.msg,
        sensor.humidity,
        sensor.temperature);

     TEST BLOCK END */

    while (!stop) {
        tuya_agent_loop();
        sleep(1);
    }

end:
    disconnect_ubus();
    tuya_agent_deinit();

    if (err == OK_T)
        syslog(LOG_INFO, "Application stopped without error");
    else 
        syslog(LOG_ERR, "Application stopped with code %s (%d)", error_to_string(err), err);

    closelog();

    return err;
}