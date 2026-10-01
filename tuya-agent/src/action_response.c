#include <syslog.h>

#include "action_response.h"
#include "tuya_agent.h"

int tuyalink_message_send(tuya_mqtt_context_t *context, tuyalink_message_t *message);

void send_action_response(const tuyalink_message_t *request, const char *response_json)
{
    syslog(
    LOG_INFO,
    "Action response msgId: %s",
    request->msgid);

syslog(
    LOG_INFO,
    "Action response payload: %s",
    response_json);

    tuyalink_message_t response = {
        .type = THING_TYPE_ACTION_EXECUTE_RSP,
        .device_id = request->device_id,
        .msgid = request->msgid,
        .data_string = (char *)response_json,
        .ack = false
    };

    int ret = tuyalink_message_send(tuya_agent_get_context(), &response);

    syslog(
    LOG_INFO,
    "Action response send ret=%d",
    ret);
    
    syslog(LOG_INFO, "Action response sent (%d)", ret);
}