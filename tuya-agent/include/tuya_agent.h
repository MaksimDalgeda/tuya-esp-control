#ifndef TUYA_AGENT_H
#define TUYA_AGENT_H

#include "tuya_agent_errors.h"
#include "tuya_agent.h"
#include "system_info_service.h"
#include "tuyalink_core.h"

Error tuya_agent_init(const char *deviceId,const char *deviceSecret);
Error tuya_agent_connect(void);
void on_connected(tuya_mqtt_context_t *context, void *user_data);
void on_disconnect(tuya_mqtt_context_t *context, void *user_data);
bool tuya_agent_is_connected(void);
void tuya_agent_loop(void);

Error tuya_agent_send(const tuya_system_info_t *message);

void tuya_agent_deinit();
#endif