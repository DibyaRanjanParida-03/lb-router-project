#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include "lb_router/NetlinkBridge.hpp"

std::atomic<bool> keep_running{true};

void signalHandler(int signum) {
    std::cout << "\n[Daemon] Interrupt signal (" << signum << ") received. Shutting down gracefully...\n";
    keep_running = false;
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "[Daemon] Starting C++ Control Plane...\n";

    try {
        lb_router::NetlinkBridge kernel_bridge;
        
        // Prototype Loop
        while (keep_running) {
            // Mocking a health check update
            kernel_bridge.sendUpdate(0xC0A8010A, true); // 192.168.1.10
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    } catch (const std::exception& e) {
        std::cerr << "[Daemon] Fatal Error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "[Daemon] Clean shutdown complete.\n";
    return 0;
}
