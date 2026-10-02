#pragma once
#include <stdint.h>
#define NETLINK_USER 31

struct nl_update_payload {
    uint32_t healthy_ip;
    uint8_t is_healthy;
};
