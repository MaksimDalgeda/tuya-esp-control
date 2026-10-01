#include <string.h>
#include <syslog.h>

#include <cjson/cJSON.h>

#include "action_handler.h"
#include "esp_controller_service.h"
#include "action_response.h"

static void execute_action(const tuyalink_message_t *msg, const cJSON *root);
static void handle_get_devices(const tuyalink_message_t *msg);
static void handle_pin_on(const tuyalink_message_t *msg, const cJSON *input_params);
static void handle_pin_off(const tuyalink_message_t *msg, const cJSON *input_params);
static void handle_read_sensor(const tuyalink_message_t *msg, const cJSON *input_params);

void handle_action_execute(const tuyalink_message_t *msg)
{
    if (msg == NULL || msg->data_string == NULL) {
        syslog(LOG_WARNING, "Action handler received NULL message");
        return;
    }

    syslog(LOG_INFO, "Received action: %s", msg->data_string);

    cJSON *root = cJSON_Parse(msg->data_string);

    if (root == NULL) {
        syslog(LOG_WARNING,"Failed to parse action JSON");
        return;
    }

    execute_action(msg, root);

    cJSON_Delete(root);
}

static void execute_action(const tuyalink_message_t *msg, const cJSON *root)
{
    cJSON *action_code =cJSON_GetObjectItem(root, "actionCode");

    if (!cJSON_IsString(action_code)) {
        syslog(LOG_WARNING, "Action does not contain actionCode");
        return;
    }

    cJSON *input_params =cJSON_GetObjectItem(root, "inputParams");

    if (input_params == NULL) {
        syslog(LOG_WARNING, "Action does not contain inputParams");
        return;
    }

    syslog(LOG_INFO, "Executing action: %s", action_code->valuestring);

    if (strcmp(action_code->valuestring, "GetDevices") == 0)
        handle_get_devices(msg);

    else if (strcmp(action_code->valuestring,"PinOn") == 0)
        handle_pin_on(msg, input_params);

    else if (strcmp(action_code->valuestring, "PinOff") == 0)
        handle_pin_off(msg, input_params);

     else if (strcmp(action_code->valuestring,"ReadSensor") == 0)
        handle_read_sensor(msg, input_params);
    
    else
        syslog(LOG_WARNING, "Unknown action: %s", action_code->valuestring);
}

static void handle_pin_on(const tuyalink_message_t *msg, const cJSON *input_params)
{
    cJSON *port = cJSON_GetObjectItem(input_params,"port");
    cJSON *pin = cJSON_GetObjectItem(input_params,"pin");

    if (!cJSON_IsString(port) || !cJSON_IsNumber(pin)) {
        syslog(LOG_WARNING,"PinOn invalid parameters");
        return;
    }

    esp_response_t response;
    Error_Code err = esp_controller_turn_pin_on(port->valuestring, pin->valueint,&response);

    if (err != OK) {
        syslog(LOG_ERR, "PinOn failed (%d)", err);
        return;
    }

    if (response.error_code != 0) {
        syslog(LOG_ERR, "PinOn error: %s (%d)", response.error_message, response.error_code);
        
        char json[256];
        snprintf(json, sizeof(json), "{" "\"outputParams\":{" "\"Message\":\"%s\"" "}" "}", response.error_message);
        send_action_response(msg, json);

        return;
    }
    syslog(LOG_INFO, "PinOn success: %s", response.msg);

    char json[256];
    snprintf(json, sizeof(json), "{" "\"outputParams\":{" "\"Message\":\"%s\"" "}" "}", response.error_message);
    send_action_response(msg, json);

    return;
}

static void handle_pin_off(const tuyalink_message_t *msg, const cJSON *input_params)
{
    cJSON *port =cJSON_GetObjectItem(input_params, "port");
    cJSON *pin = cJSON_GetObjectItem(input_params, "pin");

    if (!cJSON_IsString(port) || !cJSON_IsNumber(pin)) {
        syslog(LOG_WARNING, "PinOff invalid parameters");
        return;
    }

    esp_response_t response;
    Error_Code err = esp_controller_turn_pin_off(port->valuestring, pin->valueint, &response);

    if (err != OK) {
        syslog(LOG_ERR, "PinOff failed (%d)",err);
        return;
    }

    if (response.error_code != 0) {
        syslog(LOG_ERR, "PinOff error: %s (%d)", response.error_message, response.error_code);

        char json[256];
        snprintf(json, sizeof(json), "{" "\"outputParams\":{" "\"Message\":\"%s\"" "}" "}", response.error_message);
        send_action_response(msg, json);

        return;
    }
    
    syslog(LOG_INFO, "PinOff success: %s",response.error_message);

    char json[256];

    snprintf(json, sizeof(json), "{" "\"outputParams\":{" "\"Message\":\"%s\"" "}" "}", response.error_message);
    send_action_response(msg, json);
    return;

}

static void handle_get_devices(const tuyalink_message_t *msg)
{
    esp_device_t devices[10];
    size_t count = 0;

    Error_Code err = esp_controller_get_devices(devices, &count);

    if (err != OK) {
        syslog(LOG_ERR, "GetDevices failed (%d)", err);

        send_action_response(msg, "{" "\"outputParams\":{" "\"Message\":\"GetDevices failed\"" "}" "}");
        return;
    }

    if (count == 0) {
        syslog(LOG_WARNING, "No ESP devices found");
        send_action_response(msg, "{" "\"outputParams\":{" "\"Message\":\"No devices found\"" "}" "}");
        return;
    }

    syslog(LOG_INFO, "Found %zu ESP device(s)", count);

    char devices_str[1024] = {0};

    for (size_t i = 0; i < count; i++) {

        syslog(LOG_INFO, "Device[%zu]: port=%s vid=%s pid=%s", i, devices[i].port, devices[i].vendor_id,  devices[i].product_id);

        char entry[128];

        snprintf(entry, sizeof(entry), "%s (VID=%s PID=%s)%s",
            devices[i].port,
            devices[i].vendor_id,
            devices[i].product_id,
            (i + 1 < count) ? ", " : "");

        strncat(devices_str, entry, sizeof(devices_str) - strlen(devices_str) - 1);
    }

    char json[1200];

    snprintf(json, sizeof(json),
        "{"
        "\"outputParams\":{"
        "\"Message\":\"Devices found\","
        "\"Devices\":\"%s\""
        "}"
        "}", devices_str);

    send_action_response(msg,json);
}

static void handle_read_sensor(const tuyalink_message_t *msg, const cJSON *input_params)
{
    cJSON *port = cJSON_GetObjectItem(input_params, "port");
    cJSON *pin =cJSON_GetObjectItem(input_params, "pin");

    cJSON *model = cJSON_GetObjectItem(input_params,"model");

    cJSON *sensor = cJSON_GetObjectItem(input_params,"sensor");

    if (!cJSON_IsString(port) || !cJSON_IsNumber(pin) || !cJSON_IsString(model) || !cJSON_IsString(sensor))
    {
        syslog(LOG_WARNING, "ReadSensor invalid parameters");
        return;
    }

    dht_response_t response;

    Error_Code err = esp_controller_read_sensor(port->valuestring, pin->valueint, model->valuestring, sensor->valuestring, &response);

    if (err != OK){
        syslog(LOG_ERR, "ReadSensor failed (%d)", err);
        return;
    }

    if (response.error_code != 0){
        syslog(LOG_ERR, "ReadSensor error: %s (%d)", response.error_message, response.error_code);

        char json[256];
        snprintf(json, sizeof(json), "{" "\"outputParams\":{" "\"Message\":\"%s\"" "}" "}", response.error_message);
        send_action_response(msg, json);

        return;
    }

    syslog(LOG_INFO, "ReadSensor success: %s", response.msg);

    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"outputParams\":{"
        "\"Message\":\"%s\","
        "\"Temperature\":%.1f,"
        "\"Humidity\":%.1f"
        "}"
        "}",
        response.msg, response.temperature, response.humidity);

    send_action_response(msg, json);

    return;

}