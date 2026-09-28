#include <string.h>

#include <libubus.h>
#include <libubox/blobmsg.h>

#include "ubus_internal.h"

enum {
    DEVICE_STATISTICS,
    __DEVICE_MAX
};

static const struct blobmsg_policy device_policy[] = {
    [DEVICE_STATISTICS] = {
        .name = "statistics",
        .type = BLOBMSG_TYPE_TABLE,
    }
};

enum {
    STATS_RX_BYTES,
    STATS_TX_BYTES,
    __STATS_MAX
};

static const struct blobmsg_policy stats_policy[] = {
    [STATS_RX_BYTES] = {
        .name = "rx_bytes",
        .type = BLOBMSG_TYPE_INT64,
    },
    [STATS_TX_BYTES] = {
        .name = "tx_bytes",
        .type = BLOBMSG_TYPE_INT64,
    }
};

static void traffic_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    (void)type;

    network_info_t *network = req->priv;

    struct blob_attr *tb[__DEVICE_MAX] = {0};

    blobmsg_parse(device_policy,__DEVICE_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[DEVICE_STATISTICS])
        return;

    struct blob_attr *stats_tb[__STATS_MAX] = {0};

    blobmsg_parse(stats_policy, __STATS_MAX, stats_tb, blobmsg_data(tb[DEVICE_STATISTICS]), blobmsg_data_len(tb[DEVICE_STATISTICS]));

    if (stats_tb[STATS_RX_BYTES])
        network->rx_bytes = blobmsg_get_u64(stats_tb[STATS_RX_BYTES]);

    if (stats_tb[STATS_TX_BYTES])
        network->tx_bytes = blobmsg_get_u64(stats_tb[STATS_TX_BYTES]);

}

Error_Code ubus_get_system_traffic(network_info_t *network, size_t *network_count)
{
    int err;

    struct blob_buf b = {0};

    if (network == NULL || network_count == NULL) {
        syslog(LOG_ERR, "Invalid traffic arguments");
        return ERROR;
    }

    Ubus_State *ubus = get_ubus_state();

    if (ubus == NULL || ubus->ctx == NULL) {
        syslog(LOG_ERR, "UBUS not initialized");
        return ERR_UBUS_NOT_INITIALIZED;
    }

    if (ubus->network_device_id == 0) {
        syslog(LOG_ERR, "network.device object not found");
        return ERR_UBUS_NETWORK_DEVICE_LOOKUP;
    }

    for (size_t i = 0; i < *network_count; i++) {

        if (network[i].name[0] == '\0') {
            syslog(LOG_ERR, "Network interface name is empty");
            return ERR_UBUS_INVOKE;
        }

        blob_buf_init(&b, 0);

        blobmsg_add_string(&b,"name", network[i].name);

        network[i].rx_bytes = 0;
        network[i].tx_bytes = 0;

        err = ubus_invoke(ubus->ctx, ubus->network_device_id,  "status", b.head,  traffic_cb, &network[i],3000);

        blob_buf_free(&b);

        if (err != UBUS_STATUS_OK) {
            syslog(LOG_ERR, "Failed to get traffic statistics for interface %s (%d)", network[i].name, err);
            return ERR_UBUS_INVOKE;
        }

        if (network[i].rx_bytes == 0 && network[i].tx_bytes == 0) {
            syslog(LOG_WARNING, "Interface %s reports zero traffic", network[i].name);
        }
    }
    return OK;
}