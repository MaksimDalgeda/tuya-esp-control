#include <string.h>
#include <stdio.h>
#include <libubus.h>
#include <libubox/blobmsg.h>

#include "ubus_internal.h"

enum {
    SYSTEM_INFO_UPTIME,
    SYSTEM_INFO_MEMORY,
    __SYSTEM_INFO_MAX
};

static const struct blobmsg_policy system_info_policy[] = {
    [SYSTEM_INFO_UPTIME] = {
        .name = "uptime",
        .type = BLOBMSG_TYPE_INT32,
    },
    [SYSTEM_INFO_MEMORY] = {
        .name = "memory",
        .type = BLOBMSG_TYPE_TABLE,
    }
};

enum {
    MEMORY_TOTAL,
    MEMORY_FREE,
    __MEMORY_MAX
};

static const struct blobmsg_policy memory_policy[] = {
    [MEMORY_TOTAL] = {
        .name = "total",
        .type = BLOBMSG_TYPE_INT64,
    },
    [MEMORY_FREE] = {
        .name = "free",
        .type = BLOBMSG_TYPE_INT64,
    }
};

static void system_info_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    (void)type;

    system_info_t *info = req->priv;

    struct blob_attr *tb[__SYSTEM_INFO_MAX] = {0};
    struct blob_attr *memory_tb[__MEMORY_MAX] = {0};

    blobmsg_parse(system_info_policy, __SYSTEM_INFO_MAX, tb, blob_data(msg), blob_len(msg));

    if (tb[SYSTEM_INFO_UPTIME])
        info->uptime_s = blobmsg_get_u32(tb[SYSTEM_INFO_UPTIME]);

    if (tb[SYSTEM_INFO_MEMORY]){
        blobmsg_parse(memory_policy, __MEMORY_MAX, memory_tb, blobmsg_data(tb[SYSTEM_INFO_MEMORY]), blobmsg_data_len(tb[SYSTEM_INFO_MEMORY]));

        if (memory_tb[MEMORY_TOTAL])
            info->total_ram_bytes = blobmsg_get_u64(memory_tb[MEMORY_TOTAL]);

        if (memory_tb[MEMORY_FREE])
            info->free_ram_bytes = blobmsg_get_u64(memory_tb[MEMORY_FREE]);
    }
}


Error_Code ubus_get_system_info(system_info_t *info){

    int err;

    Ubus_State *ubus = get_ubus_state();
    if(ubus == NULL || ubus->ctx == NULL){
        syslog(LOG_ERR, "UBUS not initialized");
        return ERR_UBUS_NOT_INITIALIZED;
    }
    if(ubus->system_id == 0){
        syslog(LOG_ERR, "UBUS system object lookup failed");
        return ERR_UBUS_SYSTEM_LOOKUP;
    }

    err = ubus_invoke(ubus->ctx, ubus->system_id, "info", NULL, system_info_cb, info, 3000);

    if(err != UBUS_STATUS_OK){
        syslog(LOG_ERR, "UBUS invoke system.info failed (%d)", err);
        return ERR_UBUS_INVOKE;
    }

    return OK;
}