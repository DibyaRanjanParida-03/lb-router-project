#include "lb_router/NetlinkBridge.hpp"
#include "lb_router/lb_netlink.h"
#include <iostream>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#include <cstring>

namespace lb_router {
    NetlinkBridge::NetlinkBridge() {
        sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
        if (sock_fd < 0) {
            std::cerr << "[NetlinkBridge] Warning: Real kernel Netlink socket unavailable.\n";
        } else {
            std::cout << "[NetlinkBridge] Connected to Kernel Data Plane successfully.\n";
        }
    }
    
    NetlinkBridge::~NetlinkBridge() {
        if (sock_fd >= 0) close(sock_fd);
    }
    
    bool NetlinkBridge::sendUpdate(uint32_t ip_address, bool is_healthy) {
        if (sock_fd < 0) return false; // Fail silently if socket isn't open

        struct sockaddr_nl dest_addr;
        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.nl_family = AF_NETLINK;
        dest_addr.nl_pid = 0; // Target the kernel
        dest_addr.nl_groups = 0;

        struct {
            struct nlmsghdr nlh;
            struct nl_update_payload payload;
        } req;

        memset(&req, 0, sizeof(req));
        req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct nl_update_payload));
        req.nlh.nlmsg_pid = getpid();
        req.nlh.nlmsg_flags = 0;
        req.payload.healthy_ip = ip_address;
        req.payload.is_healthy = is_healthy ? 1 : 0;

        struct iovec iov = { &req.nlh, req.nlh.nlmsg_len };
        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_name = &dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        sendmsg(sock_fd, &msg, 0);
        return true;
    }
}
