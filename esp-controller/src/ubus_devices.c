#include <string.h>
#include <syslog.h>

#include <libubus.h>
#include <libubox/blobmsg.h>

#include "ubus_internal.h"
#include "esp_device.h"

enum {
    DEVICES,
    __DEVICES_MAX
};

static const struct blobmsg_policy devices_policy[] = {
    [DEVICES] = {
        .name = "devices",
        .type = BLOBMSG_TYPE_ARRAY,
    }
};

enum {
    DEVICE_PORT,
    DEVICE_VENDOR_ID,
    DEVICE_PRODUCT_ID,
    __DEVICE_MAX
};

static const struct blobmsg_policy device_policy[] = {
    [DEVICE_PORT] = {
        .name = "port",
        .type = BLOBMSG_TYPE_STRING,
    },

    [DEVICE_VENDOR_ID] = {
        .name = "vendor_id",
        .type = BLOBMSG_TYPE_STRING,
    },

    [DEVICE_PRODUCT_ID] = {
        .name = "product_id",
        .type = BLOBMSG_TYPE_STRING,
    }
};

typedef struct
{
    esp_device_t *devices;
    size_t *device_count;

} Devices_Context;

static void devices_cb(
    struct ubus_request *req,
    int type,
    struct blob_attr *msg)
{
    (void)type;

    Devices_Context *context = req->priv;

    struct blob_attr *tb[__DEVICES_MAX] = {0};

    blobmsg_parse(
        devices_policy,
        __DEVICES_MAX,
        tb,
        blob_data(msg),
        blob_len(msg));

    if (tb[DEVICES] == NULL)
        return;

    struct blob_attr *cur;
    int rem;

    size_t count = 0;

    blobmsg_for_each_attr(cur, tb[DEVICES], rem)
    {
        struct blob_attr *device_tb[__DEVICE_MAX] = {0};

        blobmsg_parse(
            device_policy,
            __DEVICE_MAX,
            device_tb,
            blobmsg_data(cur),
            blobmsg_data_len(cur));

        if (device_tb[DEVICE_PORT]) {

            strncpy(
                context->devices[count].port,
                blobmsg_get_string(device_tb[DEVICE_PORT]),
                sizeof(context->devices[count].port) - 1);

            context->devices[count].port[
                sizeof(context->devices[count].port) - 1] = '\0';
        }

        if (device_tb[DEVICE_VENDOR_ID]) {

            strncpy(
                context->devices[count].vendor_id,
                blobmsg_get_string(device_tb[DEVICE_VENDOR_ID]),
                sizeof(context->devices[count].vendor_id) - 1);

            context->devices[count].vendor_id[
                sizeof(context->devices[count].vendor_id) - 1] = '\0';
        }

        if (device_tb[DEVICE_PRODUCT_ID]) {

            strncpy(
                context->devices[count].product_id,
                blobmsg_get_string(device_tb[DEVICE_PRODUCT_ID]),
                sizeof(context->devices[count].product_id) - 1);

            context->devices[count].product_id[
                sizeof(context->devices[count].product_id) - 1] = '\0';
        }

        count++;
    }

    *context->device_count = count;
}

Error_Code ubus_get_devices(
    esp_device_t *devices,
    size_t *device_count)
{
    int err;

    if (devices == NULL || device_count == NULL)
        return ERROR;

    Ubus_State *ubus = get_ubus_state();

    if (ubus == NULL || ubus->ctx == NULL) {
        syslog(LOG_ERR, "UBUS not initialized");
        return ERR_UBUS_NOT_INITIALIZED;
    }

    if (ubus->esp_controller_id == 0) {
        syslog(LOG_ERR, "esp-controller object not found");
        return ERR_UBUS_ESP_CONTROLLER_LOOKUP;
    }

    *device_count = 0;

    Devices_Context context = {
        .devices = devices,
        .device_count = device_count
    };

    err = ubus_invoke(
        ubus->ctx,
        ubus->esp_controller_id,
        "devices",
        NULL,
        devices_cb,
        &context,
        3000);

    if (err != UBUS_STATUS_OK) {
        syslog(LOG_ERR,
               "Failed to invoke esp-controller devices (%d)",
               err);

        return ERR_UBUS_INVOKE;
    }

    return OK;
}