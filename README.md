# Software-Defined Load Balancer & Health-Check Router

## Stage 1: Project Introduction

### Objective
To develop a high-performance Layer 4 (TCP/UDP) Load Balancer utilizing a native Linux Kernel Module for the data plane and a modern C++20 daemon for the control plane.

### The Problem
Traditional user-space load balancers suffer from high latency and CPU overhead due to continuous context-switching between kernel space and user space when forwarding network packets.

### The Solution
This project eliminates context-switching bottlenecks by pushing packet-forwarding logic (Network Address Translation) directly into the kernel using Netfilter hooks. The complex business logic, including asynchronous backend health checks and configuration management, is handled safely in user space by a multithreaded C++ daemon. The two components communicate seamlessly via Netlink sockets.

### Project Scope
* **User Space (Control Plane):** A C++20 application utilizing asynchronous I/O (`epoll`) to monitor backend server health and push routing updates to the kernel.
* **Kernel Space (Data Plane):** A Linux Device Driver (Kernel module) utilizing Netfilter to intercept, modify, and route TCP packets in real-time.
