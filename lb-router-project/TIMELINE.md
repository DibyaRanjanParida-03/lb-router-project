# Development Plan & Timeline

## Phase 1: Planning & Architecture
* Stage 1: Project introduction and GitHub setup.
* Stage 2: Requirements gathering (PRD) and timeline definition.
* Stage 3: System architecture, UML diagrams, and project scaffolding.

## Phase 2: Core Development
* Stage 4a: C++ Control Plane - Implement the `BackendNode` classes and asynchronous `HealthMonitor`.
* Stage 4b: Kernel Data Plane - Write the basic Netfilter interceptor module and Makefile.
* Stage 4c: IPC Layer - Develop the C++ Netlink Bridge to communicate with the kernel driver.

## Phase 3: Integration & Testing
* Stage 5a: Kernel DNAT - Implement IP and Checksum rewriting inside the Kernel module.
* Stage 5b: Testing - Write GTest suites for C++ classes. Perform Valgrind memory leak tests.
* Stage 5c: Stress Testing - Use `iperf3`/`wrk` to validate load balancer under heavy traffic.

## Phase 4: Delivery
* Stage 6: Final implementation polish, graceful daemon shutdown (SIGINT handling), live demonstration.
