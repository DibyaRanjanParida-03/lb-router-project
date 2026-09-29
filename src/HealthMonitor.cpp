#include "lb_router/HealthMonitor.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

namespace lb_router {

HealthMonitor::HealthMonitor(std::vector<std::string> ips, uint16_t p, std::atomic<bool>& running, NetlinkBridge& b)
    : backend_ips(ips), port(p), keep_running(running), bridge(b) {
    epoll_fd = epoll_create1(0);
    monitor_thread = std::thread(&HealthMonitor::run, this);
}

HealthMonitor::~HealthMonitor() {
    if (monitor_thread.joinable()) {
        monitor_thread.join();
    }
    close(epoll_fd);
}

void HealthMonitor::run() {
    while (keep_running) {
        std::map<int, uint32_t> fd_to_ip;
        
        for (const auto& ip : backend_ips) {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            fcntl(sock, F_SETFL, O_NONBLOCK);
            
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

            connect(sock, (struct sockaddr*)&addr, sizeof(addr));
            fd_to_ip[sock] = addr.sin_addr.s_addr;

            epoll_event ev{};
            ev.events = EPOLLOUT;
            ev.data.fd = sock;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev);
        }

        epoll_event events[10];
        int nfds = epoll_wait(epoll_fd, events, 10, 2000);

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            int error = 0;
            socklen_t len = sizeof(error);
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);

            bridge.sendUpdate(fd_to_ip[fd], (error == 0)); 
            
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

}
