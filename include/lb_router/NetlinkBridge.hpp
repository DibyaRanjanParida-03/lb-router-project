#pragma once
#include <stdint.h>

namespace lb_router {
    class NetlinkBridge {
    private:
        int sock_fd;
    public:
        NetlinkBridge();
        ~NetlinkBridge();
        bool sendUpdate(uint32_t ip_address, bool is_healthy);
    };
}
