# Product Requirements Document (PRD)
**Project:** Software-Defined Load Balancer & Health-Check Router

## 1. System Overview
A high-throughput, dual-plane Layer 4 load balancer. The data plane resides in the Linux Kernel to process packets at line rate, while the control plane runs as a user-space C++20 daemon to handle routing logic and health monitoring.

## 2. Functional Requirements
* **Kernel Data Plane (Netfilter):**
  * Must intercept incoming TCP packets using `NF_INET_PRE_ROUTING`.
  * Must perform Destination Network Address Translation (DNAT) to route packets to backend IPs.
  * Must dynamically receive and parse routing table updates via Netlink sockets.
* **C++ Control Plane (Daemon):**
  * Must actively probe backend server health via asynchronous TCP socket connections (using `epoll` or standard C++ threads).
  * Must identify when a node goes offline and dynamically update the kernel routing table.
  * Must serialize routing commands and communicate securely with the kernel via Netlink.

## 3. Non-Functional Requirements
* **Performance:** Minimal packet processing latency; no kernel-to-user-space copying of payload data.
* **Memory Safety:** The user-space daemon must strictly utilize modern C++ (RAII, smart pointers) to guarantee zero memory leaks. Legacy C memory management is forbidden in user-space.
* **Kernel Stability:** The driver must utilize safe spinlocks for routing table modifications to prevent kernel panics.

## 4. Deliverables
1. C++20 Control Daemon Source Code.
2. Linux Kernel Module (`.ko`) Source Code.
3. UML Architecture Diagrams.
4. Comprehensive Test Suite (GTest).
