#pragma once

#include <linux/types.h>

#define NETLINK_USER 31

struct lb_cmd {
    __u32 ip_address;
    __u8 is_healthy;
};
