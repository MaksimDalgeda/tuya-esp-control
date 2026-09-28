#include <string.h>
#include <stdio.h>

#include <libubus.h>
#include <libubox/blobmsg.h>

#include "ubus_internal.h"

enum {
    NETWORK_DEVICE,
    NETWORK_IPV4_ADDRESS,
    __NETWORK_MAX
};

static const struct blobmsg_policy network_policy[] = {
    [NETWORK_DEVICE] = {
        .name = "device",
        .type = BLOBMSG_TYPE_STRING,
    },
    [NETWORK_IPV4_ADDRESS] = {
        .name = "ipv4-address",
        .type = BLOBMSG_TYPE_ARRAY,
    }
};

enum {
    IPV4_ADDRESS,
    IPV4_MASK,
    __IPV4_MAX
};

static const struct blobmsg_policy ipv4_policy[] = {
    [IPV4_ADDRESS] = {
        .name = "address",
        .type = BLOBMSG_TYPE_STRING,
    },
    [IPV4_MASK] = {
        .name = "mask",
        .type = BLOBMSG_TYPE_INT32,
    }
};

static void prefix_to_netmask(uint32_t prefix, char *buf, size_t len)
{
    uint32_t mask;

    if(prefix == 0)
        mask = 0;
    else
        mask = 0xFFFFFFFF << (32 - prefix);

    snprintf(buf, len, "%u.%u.%u.%u", (mask >> 24) & 0xFF, (mask >> 16) & 0xFF, (mask >> 8) & 0xFF, mask & 0xFF);
}

static void network_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    (void)type;

    network_info_t *network = req->priv;

    struct blob_attr *tb[__NETWORK_MAX] = {0};

    blobmsg_parse(network_policy,__NETWORK_MAX, tb, blob_data(msg), blob_len(msg));

    if (tb[NETWORK_DEVICE]) {
        const char *device = blobmsg_get_string(tb[NETWORK_DEVICE]);

        strncpy(network->name, device, sizeof(network->name) - 1);

        network->name[sizeof(network->name) - 1] = '\0';
    }

    
    if(tb[NETWORK_IPV4_ADDRESS]){
        struct blob_attr *cur;
        int rem;

        blobmsg_for_each_attr(cur, tb[NETWORK_IPV4_ADDRESS], rem){
            struct blob_attr *ip_tb[__IPV4_MAX] = {0};

            blobmsg_parse(ipv4_policy, __IPV4_MAX, ip_tb, blobmsg_data(cur), blobmsg_data_len(cur));

            if(ip_tb[IPV4_ADDRESS])
                strncpy(network->ip, blobmsg_get_string(ip_tb[IPV4_ADDRESS]), sizeof(network->ip) - 1);
            

            if(ip_tb[IPV4_MASK])
                prefix_to_netmask(blobmsg_get_u32(ip_tb[IPV4_MASK]), network->netmask, sizeof(network->netmask));

            break;
        }
    }
}

Error_Code ubus_get_system_network(network_info_t *network, size_t *network_count)
{
    int err;

    if (network == NULL || network_count == NULL) {
        syslog(LOG_ERR, "Invalid network arguments");
        return ERROR;
    }
    err = get_network_interfaces(network, network_count);
    if (err != OK)
        return err;

    Ubus_State *ubus = get_ubus_state();

    if (ubus == NULL || ubus->ctx == NULL) {
        syslog(LOG_ERR, "UBUS not initialized");
        return ERR_UBUS_NOT_INITIALIZED;
    }

    for (size_t i = 0; i < *network_count; i++) {

        char object[64];
        uint32_t object_id;

        snprintf(object,sizeof(object), "network.interface.%s", network[i].name);

        err = ubus_lookup_id(ubus->ctx, object, &object_id);

        if (err != UBUS_STATUS_OK) {
            syslog(LOG_WARNING, "Interface %s not found", network[i].name);
            continue;
        }

        err = ubus_invoke(ubus->ctx, object_id, "status", NULL, network_cb, &network[i], 3000);

        if (err != UBUS_STATUS_OK)
            continue;
    }
   
    return OK;
}