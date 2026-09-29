#include <iostream>
#include <csignal>
#include <atomic>
#include <vector>
#include <string>
#include "lb_router/NetlinkBridge.hpp"
#include "lb_router/HealthMonitor.hpp"

std::atomic<bool> keep_running{true};

void signalHandler(int signum) {
    keep_running = false;
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        lb_router::NetlinkBridge kernel_bridge;
        std::vector<std::string> backends = {"192.168.1.10", "192.168.1.11"};
        
        lb_router::HealthMonitor monitor(backends, 80, keep_running, kernel_bridge);
        
        while (keep_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        return 1;
    }

    return 0;
}
