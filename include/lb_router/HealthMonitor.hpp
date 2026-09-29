#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <sys/epoll.h>
#include "lb_router/NetlinkBridge.hpp"

namespace lb_router {

class HealthMonitor {
private:
    int epoll_fd;
    std::vector<std::string> backend_ips;
    uint16_t port;
    std::atomic<bool>& keep_running;
    NetlinkBridge& bridge;
    std::thread monitor_thread;

    void run();

public:
    HealthMonitor(std::vector<std::string> ips, uint16_t p, std::atomic<bool>& running, NetlinkBridge& b);
    ~HealthMonitor();
};

}
