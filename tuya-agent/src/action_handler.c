#include <string.h>
#include <syslog.h>

#include <cjson/cJSON.h>

#include "action_handler.h"
#include "esp_controller_service.h"

static void execute_action(const cJSON *root);
static void handle_get_devices(void);
static void handle_pin_on(const cJSON *input_params);
static void handle_pin_off(const cJSON *input_params);
static void handle_read_sensor(const cJSON *input_params);

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

    execute_action(root);

    cJSON_Delete(root);
}

static void execute_action(const cJSON *root)
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
        handle_get_devices();

    else if (strcmp(action_code->valuestring,"PinOn") == 0)
        handle_pin_on(input_params);

    else if (strcmp(action_code->valuestring, "PinOff") == 0)
        handle_pin_off(input_params);

     else if (strcmp(action_code->valuestring,"ReadSensor") == 0)
        handle_read_sensor(input_params);
    
    else
        syslog(LOG_WARNING, "Unknown action: %s", action_code->valuestring);
}

static void handle_pin_on(const cJSON *input_params)
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
        return;
    }
    syslog(LOG_INFO, "PinOn success: %s", response.msg);
}

static void handle_pin_off(const cJSON *input_params)
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
        return;
    }
    
    syslog(LOG_INFO, "PinOff success: %s",response.msg);
}

static void handle_get_devices(void)
{
    esp_device_t devices[10];
    size_t count = 0;

    Error_Code err;

    err = esp_controller_get_devices(devices, &count);

    if (err != OK) {
        syslog(LOG_ERR, "GetDevices failed (%d)", err);
        return;
    }

    if (count == 0) {
        syslog(LOG_WARNING, "No ESP devices found");
        return;
    }

    syslog(LOG_INFO, "Found %zu ESP device(s)",count);

    for (size_t i = 0; i < count; i++) {

        syslog(LOG_INFO, "Device[%zu]: port=%s vid=%s pid=%s", i,
            devices[i].port,
            devices[i].vendor_id,
            devices[i].product_id);
    }
}

static void handle_read_sensor(const cJSON *input_params)
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

    Error_Code err = esp_controller_read_sensor(port->valuestring, pin->valueint, model->valuestring,
            sensor->valuestring,
            &response);

    if (err != OK)
    {
        syslog(
            LOG_ERR,
            "ReadSensor failed (%d)",
            err);

        return;
    }

    if (response.error_code != 0)
    {
        syslog(
            LOG_ERR,
            "ReadSensor error: %s (%d)",
            response.error_message,
            response.error_code);

        return;
    }

    syslog(
        LOG_INFO,
        "ReadSensor success: %s",
        response.msg);

    syslog(
        LOG_INFO,
        "Temperature: %.1f C, Humidity: %.1f %%",
        response.temperature,
        response.humidity);
}