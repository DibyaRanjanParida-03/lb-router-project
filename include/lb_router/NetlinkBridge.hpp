#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

namespace lb_router {

class NetlinkBridge {
private:
    int sock_fd;
    sockaddr_nl src_addr;
    sockaddr_nl dest_addr;

public:
    NetlinkBridge();
    ~NetlinkBridge();
    
    // Prevent copying to maintain strict ownership of the socket file descriptor
    NetlinkBridge(const NetlinkBridge&) = delete;
    NetlinkBridge& operator=(const NetlinkBridge&) = delete;

    bool sendUpdate(uint32_t ip_address, bool is_healthy);
};

} // namespace lb_router
