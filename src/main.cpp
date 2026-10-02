#include "lb_router/HealthMonitor.hpp"
#include "lb_router/NetlinkBridge.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> keep_running{true};

// Catch Ctrl+C to shut down safely
void signal_handler(int signum) {
    std::cout << "\n[Control Plane] Shutting down gracefully...\n";
    keep_running = false;
}

int main() {
    signal(SIGINT, signal_handler);

    std::cout << "[Control Plane] Starting C++ Daemon...\n";

    lb_router::NetlinkBridge bridge;
    
    // Define the backend servers we want to monitor
    std::vector<std::string> backend_ips = {"192.168.1.10", "192.168.1.11"};
    
    // Start the health monitor in a background thread
    lb_router::HealthMonitor monitor(backend_ips, 80, keep_running, bridge);

    // Keep the main program alive! 
    // Without this loop, the program instantly closes.
    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
