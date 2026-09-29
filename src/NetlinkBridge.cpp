#include "lb_router/NetlinkBridge.hpp"
#include "lb_router/lb_netlink.h"
#include <cstring>
#include <stdexcept>

namespace lb_router {

NetlinkBridge::NetlinkBridge() : sock_fd(-1) {
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        throw std::runtime_error("Failed to create Netlink socket.");
    }

    std::memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); 

    if (bind(sock_fd, reinterpret_cast<struct sockaddr*>(&src_addr), sizeof(src_addr)) < 0) {
        close(sock_fd);
        throw std::runtime_error("Failed to bind Netlink socket.");
    }

    std::memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; 
    dest_addr.nl_groups = 0; 
}

NetlinkBridge::~NetlinkBridge() {
    if (sock_fd >= 0) {
        close(sock_fd);
    }
}

bool NetlinkBridge::sendUpdate(uint32_t ip_address, bool is_healthy) {
    struct {
        struct nlmsghdr nlh;
        struct lb_cmd cmd;
    } req;

    std::memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct lb_cmd));
    req.nlh.nlmsg_pid = getpid();
    req.nlh.nlmsg_flags = NLM_F_REQUEST;

    req.cmd.ip_address = ip_address;
    req.cmd.is_healthy = is_healthy ? 1 : 0;

    struct iovec iov = { &req.nlh, req.nlh.nlmsg_len };
    struct msghdr msg = {};
    msg.msg_name = &dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    return sendmsg(sock_fd, &msg, 0) >= 0;
}

}
