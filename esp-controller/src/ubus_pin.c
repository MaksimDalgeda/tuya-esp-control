#include <string.h>

#include <libubus.h>
#include <libubox/blobmsg.h>

#include "esp_device.h"
#include "esp_errors.h"
#include "ubus.h"

enum {
    RESPONSE_RC,
    RESPONSE_MSG,
    RESPONSE_ERROR,
    __RESPONSE_MAX
};

static const struct blobmsg_policy response_policy[] = {
    [RESPONSE_RC] = {
        .name = "rc",
        .type = BLOBMSG_TYPE_INT32,
    },

    [RESPONSE_MSG] = {
        .name = "msg",
        .type = BLOBMSG_TYPE_STRING,
    },

    [RESPONSE_ERROR] = {
        .name = "error",
        .type = BLOBMSG_TYPE_TABLE,
    }
};

enum {
    ERROR_CODE,
    ERROR_MESSAGE,
    __ERROR_MAX
};

static const struct blobmsg_policy error_policy[] = {
    [ERROR_CODE] = {
        .name = "code",
        .type = BLOBMSG_TYPE_INT32,
    },

    [ERROR_MESSAGE] = {
        .name = "message",
        .type = BLOBMSG_TYPE_STRING,
    }
};

static void response_cb(struct ubus_request *req, int type,struct blob_attr *msg)
{
    (void)type;

    esp_response_t *response = req->priv;

    memset(response, 0, sizeof(*response));

    struct blob_attr *tb[__RESPONSE_MAX] = {0};

    blobmsg_parse(response_policy, __RESPONSE_MAX, tb, blob_data(msg), blob_len(msg));

    if (tb[RESPONSE_RC]) {
        response->rc =blobmsg_get_u32(tb[RESPONSE_RC]);
    }

    if (tb[RESPONSE_MSG]) {
        strncpy(response->msg, blobmsg_get_string(tb[RESPONSE_MSG]), sizeof(response->msg) - 1);
        
        response->msg[sizeof(response->msg) - 1] = '\0';
    }

      if (response->rc != 0) {

            response->error_code = response->rc;

            strncpy(response->error_message, response->msg, sizeof(response->error_message) - 1);
            response->error_message[sizeof(response->error_message) - 1] = '\0';
        }

    if (tb[RESPONSE_ERROR]) {

        struct blob_attr *error_tb[__ERROR_MAX] = {0};

        blobmsg_parse(error_policy, __ERROR_MAX, error_tb,
            blobmsg_data(tb[RESPONSE_ERROR]),
            blobmsg_data_len(tb[RESPONSE_ERROR]));

        if (error_tb[ERROR_CODE]) 
            response->error_code = blobmsg_get_u32(error_tb[ERROR_CODE]);

        if (error_tb[ERROR_MESSAGE]) {

            strncpy(response->error_message, blobmsg_get_string(error_tb[ERROR_MESSAGE]),
                sizeof(response->error_message) - 1);

            response->error_message[sizeof(response->error_message) - 1] = '\0';
        }
    }
}

Error_Code ubus_pin_on(const char *port, int pin, esp_response_t *response)
{
    Ubus_State *ubus = get_ubus_state();

    if (ubus == NULL || ubus->ctx == NULL)
        return ERR_UBUS_NOT_INITIALIZED;

    struct blob_buf b = {0};

    blob_buf_init(&b, 0);

    blobmsg_add_string(&b, "port", port);

    blobmsg_add_u32(&b, "pin", pin);

    int err = ubus_invoke(ubus->ctx,ubus->esp_controller_id,"on", b.head, response_cb, response, 3000);

    blob_buf_free(&b);

    if (err != UBUS_STATUS_OK)
        return ERR_UBUS_INVOKE;

    return OK;
}

Error_Code ubus_pin_off(const char *port,int pin, esp_response_t *response)
{
    Ubus_State *ubus = get_ubus_state();
    if (ubus == NULL || ubus->ctx == NULL)
        return ERR_UBUS_NOT_INITIALIZED;

    struct blob_buf b = {0};

    blob_buf_init(&b, 0);

    blobmsg_add_string(&b, "port", port);

    blobmsg_add_u32(&b,"pin", pin);

    int err = ubus_invoke(ubus->ctx, ubus->esp_controller_id, "off", b.head, response_cb, response, 3000);

    blob_buf_free(&b);

if (err != UBUS_STATUS_OK)
       return ERR_UBUS_INVOKE;

    return OK;
}