
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

## Stage 2: Requirements & Development Plan
In this stage, the functional and non-functional requirements were established, mapping out the strict separation of concerns between user-space memory safety and kernel-space performance.

### Product Requirements Document (PRD)
**Project:** Software-Defined Load Balancer & Health-Check Router

#### 1. System Overview
A high-throughput, dual-plane Layer 4 load balancer. The data plane resides in the Linux Kernel to process packets at line rate, while the control plane runs as a user-space C++20 daemon to handle routing logic and health monitoring.

#### 2. Functional Requirements
**Kernel Data Plane (Netfilter):**
* Must intercept incoming TCP packets using `NF_INET_PRE_ROUTING`.
* Must perform Destination Network Address Translation (DNAT) to route packets to backend IPs.
* Must dynamically receive and parse routing table updates via Netlink sockets.

**C++ Control Plane (Daemon):**
* Must actively probe backend server health via asynchronous TCP socket connections (using `epoll` or standard C++ threads).
* Must identify when a node goes offline and dynamically update the kernel routing table.
* Must serialize routing commands and communicate securely with the kernel via Netlink.

#### 3. Non-Functional Requirements
* **Performance:** Minimal packet processing latency; no kernel-to-user-space copying of payload data.
* **Memory Safety:** The user-space daemon must strictly utilize modern C++ (RAII, smart pointers) to guarantee zero memory leaks. Legacy C memory management is forbidden in user-space.
* **Kernel Stability:** The driver must utilize safe spinlocks for routing table modifications to prevent kernel panics.

#### 4. Deliverables
* C++20 Control Daemon Source Code.
* Linux Kernel Module (`.ko`) Source Code.
* UML Architecture Diagrams.
* Comprehensive Test Suite (GTest).

**Development Process:**
1. **Architecture Formulation:** Component design, UML modeling, and IPC boundaries.
2. **C++ Control Plane:** Development of the C++20 daemon and asynchronous TCP health checkers.
3. **Kernel Data Plane:** Implementation of the Netfilter kernel module for packet interception and DNAT.
4. **Integration & Delivery:** Bridging user and kernel space via Netlink, stress testing, and final presentation.


## Stage 3: System Design & Architecture
The system architecture and component interactions are documented using Mermaid.js UML:
* [C++ Class Diagram](lb-router-project/diagrams/ClassDiagram.md)
* [Packet Flow Sequence Diagram](lb-router-project/diagrams/SequenceDiagram.md)
* [Backend State Machine Diagram](lb-router-project/diagrams/StateDiagram.md)


```text
+-----------------------+           +------------------------+
|     CONTROL PLANE     |           |       DATA PLANE       |
|     (User Space)      |           |     (Kernel Space)     |
|                       |  Netlink  |                        |
|  [ C++ Daemon ]       |==========>|  [ Netfilter Module ]  |
|  - Async epoll checks |   (IPC)   |  - Intercepts Traffic  |
|  - Tracks Server IPs  |           |  - Rewrites IPs (DNAT) |
+----------+------------+           +-----------+------------+
           |                                    |
           | HTTP Probes                        | TCP/UDP Traffic
           v                                    v
+------------------------------------------------------------+
|                   BACKEND WEB SERVERS                      |
|             (192.168.1.10, 192.168.1.11)                   |
+------------------------------------------------------------+
```




## Stage 4: Initial Implementation & Prototype
In this stage, the core skeletons for both the user-space and kernel-space components were implemented and successfully tested.

* **C++ Control Plane:** A modern C++20 daemon featuring graceful shutdown handling (`SIGINT`/`SIGTERM`) and an RAII-compliant `NetlinkBridge` class to manage raw socket communication safely without memory leaks.
* **Kernel Data Plane:** A basic loadable kernel module (`lb_interceptor.ko`) utilizing the `NF_INET_PRE_ROUTING` hook to intercept HTTP TCP traffic and log packet details.

**Build and Run Instructions:**
* **C++ Daemon:** `mkdir build && cd build && cmake .. && make`
* **Kernel Driver:** `cd kernel_driver && make`
* **Execution:** Insert the module with `sudo insmod lb_interceptor.ko`, then run the daemon with `sudo ./lb_daemon`.

## Stage 5: Asynchronous Health Checks & Kernel DNAT
In this stage, the core functional logic of the load balancer was fully realized.

* **C++ Control Plane:** Upgraded to use non-blocking asynchronous I/O (`epoll`) via the `HealthMonitor` class, allowing the daemon to simultaneously track the health of multiple backend servers without blocking threads.
* **Kernel Data Plane:** Implemented Destination Network Address Translation (DNAT). The Netfilter hook now safely makes the `sk_buff` writable, rewrites the destination IP for HTTP traffic (port 80), and strictly recalculates both the IP and TCP checksums to ensure packets are accepted by the target backend.

## Stage 6: Netlink IPC Integration
In this final architectural stage, the user-space and kernel-space components were bridged, allowing real-time, dynamic routing updates.

* **Shared IPC Protocol:** Introduced `lb_netlink.h` to enforce a strict memory layout for messages passed across the user/kernel boundary.
* **C++ Control Plane:** Upgraded the `NetlinkBridge` to serialize dynamically discovered healthy IP addresses into proper Netlink Message Headers (`nlmsghdr`) and push them to the kernel.
* **Kernel Data Plane:** Implemented a Netlink socket listener (`netlink_kernel_create`). The kernel module now actively listens for health updates and instantly swaps the `target_ip` used for DNAT routing, entirely eliminating hardcoded destinations.

## Execution Commands & Output

**1. Compile the Control Plane:**

g++ -std=c++20 -I./include src/main.cpp src/NetlinkBridge.cpp src/HealthMonitor.cpp -o lb_daemon -pthread

**2. Run the Daemon:**
sudo ./lb_daemon

**Live Output:**
![Load Balancer Output]:https://github.com/DibyaRanjanParida-03/lb-router-project/blob/main/Screenshot%202026-10-04%20190821.png


## Project Conclusion
This project successfully demonstrates a high-performance, hybrid-architecture load balancer. By splitting responsibilities across the operating system privilege boundary, the system achieves the best of both worlds:

1. **Data Plane Performance:** Packet interception and IP rewriting (DNAT) occur strictly in kernel space (Ring 0) using Netfilter, eliminating costly context switches and maximizing throughput.
2. **Control Plane Safety:** Complex business logic, asynchronous connection handling (`epoll`), and resource management (RAII) are handled safely in user space using modern C++20.
3. **Seamless IPC:** Real-time synchronization is maintained through lightweight Netlink sockets, allowing the routing table to adapt dynamically to backend health states.

**Future Enhancements:**
* Implementation of advanced load-balancing algorithms (e.g., Weighted Round-Robin, Least Connections).
* Expanding protocol support beyond TCP (e.g., UDP).
* Migrating the kernel module logic to eBPF/XDP for even lower latency packet processing.
