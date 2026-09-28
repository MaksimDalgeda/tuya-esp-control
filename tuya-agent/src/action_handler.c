#include <stdio.h>
#include <syslog.h>
#include "action_handler.h"

#define ACTION_LOG_FILE "/tmp/tuya_action.log"

static void save_action_text(const char *text);
static void execute_action(const cJSON *root);

void handle_action_execute(const tuyalink_message_t *msg)
{
    if (msg == NULL || msg->data_string == NULL){
        syslog(LOG_WARNING, "Action handler received NULL message");
        return;
    }

    cJSON *root = cJSON_Parse(msg->data_string);

    if (root == NULL){
        syslog(LOG_WARNING, "Failed to parse action JSON");
        return;
    }
    execute_action(root);

    cJSON_Delete(root);
}

static void execute_action(const cJSON *root)
{
    cJSON *input_params = cJSON_GetObjectItem(root, "inputParams");

    if (input_params == NULL){
        syslog(LOG_WARNING, "Action does not contain inputParams");
        return;
    }

    cJSON *text = cJSON_GetObjectItem(input_params, "text");

    if (!cJSON_IsString(text)){
        syslog(LOG_WARNING, "Action text parameter error");
        return;
    }
    
    save_action_text(text->valuestring);
}

static void save_action_text(const char *text)
{
    FILE *fp = fopen(ACTION_LOG_FILE, "a");

    if (fp == NULL){
        syslog(LOG_ERR, "Failed to open %s", ACTION_LOG_FILE);
        return;
    }

    fprintf(fp, "%s\n", text);

    syslog(LOG_INFO, "Action received: %s", text);

    fclose(fp);
}