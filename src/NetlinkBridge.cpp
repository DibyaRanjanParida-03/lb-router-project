#include "lb_router/NetlinkBridge.hpp"
#include <cstring>

#define NETLINK_USER 31

namespace lb_router {

NetlinkBridge::NetlinkBridge() : sock_fd(-1) {
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        throw std::runtime_error("Failed to create Netlink socket. Are you running as root?");
    }

    std::memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); // C++ daemon process ID

    if (bind(sock_fd, reinterpret_cast<struct sockaddr*>(&src_addr), sizeof(src_addr)) < 0) {
        close(sock_fd);
        throw std::runtime_error("Failed to bind Netlink socket.");
    }

    std::memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // 0 means Linux Kernel
    dest_addr.nl_groups = 0; // Unicast
}

NetlinkBridge::~NetlinkBridge() {
    if (sock_fd >= 0) {
        close(sock_fd);
        std::cout << "[NetlinkBridge] Socket closed safely via RAII.\n";
    }
}

bool NetlinkBridge::sendUpdate(uint32_t ip_address, bool is_healthy) {
    // We will implement the actual message construction in Stage 5.
    // For the prototype, we just prove the class works.
    std::cout << "[NetlinkBridge] Mock sending IP: " << ip_address 
              << " Status: " << (is_healthy ? "ONLINE" : "OFFLINE") << "\n";
    return true;
}

} // namespace lb_router
