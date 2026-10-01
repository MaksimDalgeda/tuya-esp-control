#ifndef ACTION_RESPONSE_H
#define ACTION_RESPONSE_H

#include "tuyalink_core.h"

void send_action_response(const tuyalink_message_t *request,const char *response_json);

#endif