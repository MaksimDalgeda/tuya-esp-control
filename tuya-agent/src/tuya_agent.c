#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include "tuya_agent_errors.h"
#include "tuyalink_core.h"
#include "tuya_cacert.h"
#include "system_info_service.h"
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

static void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg)
{
    (void)context;
    (void)user_data;

    if (msg == NULL)
        return;

    switch (msg->type){ //for future actions
        case THING_TYPE_ACTION_EXECUTE:
            handle_action_execute(msg);
            break;

        default:
            break;
    }
}

Error tuya_agent_init(const char *deviceId,const char *deviceSecret)
{   
    int ret;

    ret = tuya_mqtt_init(&client,
        &(const tuya_mqtt_config_t)
        {
            .host = "m1.tuyacn.com",
            .port = 8883,

            .cacert = (const uint8_t *)tuya_cacert_pem,
            .cacert_len = sizeof(tuya_cacert_pem),

            .device_id = deviceId,
            .device_secret = deviceSecret,

            .keepalive = 60,
            .timeout_ms = 2000,

            .on_connected = on_connected,
            .on_disconnect = on_disconnect,
            .on_messages = on_messages,
        });

    if(ret != 0){
        syslog(LOG_WARNING, "tuya_mqtt_init failed: %d\n", ret);
        return ERROR_T;
    }

    syslog(LOG_INFO, "Tuya initialized\n");

    return OK_T;
}

void tuya_agent_deinit()
{
    tuya_mqtt_disconnect(&client);
    tuya_mqtt_deinit(&client);
}


Error tuya_agent_connect(void)
{
    int ret;

    ret = tuya_mqtt_connect(&client);

    if(ret != OK_T){
        syslog(LOG_WARNING, "tuya_mqtt_connect failed: %d\n", ret);
        return ERROR_T;
    }
    return OK_T;
}

void tuya_agent_loop(void)
{
    tuya_mqtt_loop(&client);
}

Error tuya_agent_send(const tuya_system_info_t *message)
{
    char payload[1024];
    int offset = 0;

    offset += snprintf(payload + offset,
                       sizeof(payload) - offset,
                       "{"
                       "\"memory_info\":{"
                           "\"total_ram\":%.0f,"
                           "\"free_ram\":%.0f"
                       "},"
                       "\"cpu_info\":{"
                           "\"usage\":%.1f"
                       "},"
                       "\"uptime_info\":{"
                           "\"uptime\":%ld"
                       "},"
                       "\"network_interfaces\":[",
                       message->total_ram_mb,
                       message->free_ram_mb,
                       message->cpu_usage_prcnt,
                       message->uptime_s);

    for (size_t i = 0; i < message->network_count; i++){
        offset += snprintf(payload + offset,
                           sizeof(payload) - offset,
                           "{"
                           "\"interface_name\":\"%s\","
                           "\"ip_address\":\"%s\","
                           "\"netmask\":\"%s\","
                           "\"transmitted_data_amount\":%.2f,"
                           "\"received_data_amount\":%.2f"
                           "}",
                           message->network[i].name,
                           message->network[i].ip,
                           message->network[i].netmask,
                           message->network[i].tx_mb,
                           message->network[i].rx_mb);

        if (i < message->network_count - 1){
            offset += snprintf(payload + offset,sizeof(payload) - offset, ",");
        }
    }

    offset += snprintf(payload + offset, sizeof(payload) - offset, "]}");

   int report_id = tuyalink_thing_property_report(&client, NULL, payload);

   syslog(LOG_DEBUG, "Property report sent, report id: %d", report_id);

    return OK_T;
}