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

* [Product Requirements Document (PRD)](lb-router-project/PRD.md)

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
