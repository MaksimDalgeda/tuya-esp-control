#ifndef TUYA_AGENT_H
#define TUYA_AGENT_H

#include <stdbool.h>

#include "tuya_agent_errors.h"
#include "tuyalink_core.h"

Error tuya_agent_init(const char *deviceId, const char *deviceSecret);
Error tuya_agent_connect(void);
void tuya_agent_deinit(void);
void tuya_agent_loop(void);

bool tuya_agent_is_connected(void);

void on_connected(tuya_mqtt_context_t *context, void *user_data);
void on_disconnect(tuya_mqtt_context_t *context, void *user_data);

#endif