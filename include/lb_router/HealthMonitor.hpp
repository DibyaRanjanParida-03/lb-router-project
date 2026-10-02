#pragma once
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include "NetlinkBridge.hpp"

namespace lb_router {
    class HealthMonitor {
    private:
        std::vector<std::string> backend_ips;
        uint16_t port;
        std::atomic<bool>& keep_running;
        NetlinkBridge& bridge;
        int epoll_fd;
        std::thread monitor_thread;
        void run();
    public:
        HealthMonitor(std::vector<std::string> ips, uint16_t p, std::atomic<bool>& running, NetlinkBridge& b);
        ~HealthMonitor();
    };
}
