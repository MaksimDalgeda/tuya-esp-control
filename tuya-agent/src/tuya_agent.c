#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include "tuya_agent.h"
#include "tuya_cacert.h"
#include "action_handler.h"

static tuya_mqtt_context_t client;
static bool connected = false;

void on_connected(tuya_mqtt_context_t *context, void *user_data)
{
    (void)context;
    (void)user_data;

    connected = true;

    syslog(LOG_INFO, "Connected callback");
}

void on_disconnect(tuya_mqtt_context_t *context, void *user_data)
{
    (void)context;
    (void)user_data;

    connected = false;

    syslog(LOG_INFO, "Disconnected callback");
}

bool tuya_agent_is_connected(void)
{
    return connected;
}

static void on_messages(tuya_mqtt_context_t *context,void *user_data, const tuyalink_message_t *msg)
{
    (void)context;
    (void)user_data;

    if (msg == NULL)
        return;

    switch (msg->type)
    {
        case THING_TYPE_ACTION_EXECUTE:

            syslog(LOG_INFO, "Action received");

            handle_action_execute(msg);

            break;

        default:
            break;
    }
}

Error tuya_agent_init(const char *deviceId, const char *deviceSecret)
{
    int ret;

    ret = tuya_mqtt_init(
        &client,
        &(const tuya_mqtt_config_t)
        {
            .host = "m1.tuyacn.com",
            .port = 8883,

            .cacert =
                (const uint8_t *)tuya_cacert_pem,

            .cacert_len =
                sizeof(tuya_cacert_pem),

            .device_id = deviceId,
            .device_secret = deviceSecret,

            .keepalive = 60,
            .timeout_ms = 2000,

            .on_connected = on_connected,
            .on_disconnect = on_disconnect,
            .on_messages = on_messages,
        });

    if (ret != 0) {

        syslog(LOG_ERR, "tuya_mqtt_init failed (%d)",ret);

        return ERROR_T;
    }

    syslog(LOG_INFO, "Tuya initialized");

    return OK_T;
}

Error tuya_agent_connect(void)
{
    int ret;

    ret = tuya_mqtt_connect(&client);

    if (ret != 0) {

        syslog(LOG_ERR,"tuya_mqtt_connect failed (%d)",ret);

        return ERROR_CONNECT_T;
    }

    return OK_T;
}

void tuya_agent_loop(void)
{
    tuya_mqtt_loop(&client);
}

void tuya_agent_deinit(void)
{
    tuya_mqtt_disconnect(&client);
    tuya_mqtt_deinit(&client);
}