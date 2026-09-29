# Packet Flow & IPC Sequence Diag
mermaid
sequenceDiagram
participant Client
participant Kernel as Kernel Module (Netfilter)
participant CppDaemon as C++ Daemon (User Space)
participant Backend as Backend Web Servers
loop Every 5 seconds
        CppDaemon->>Backend: Asynchronous TCP Check (epoll/thread)
        alt Server Offline
            Backend-->>CppDaemon: Connection Refused / Timeout
            CppDaemon->>Kernel: Send Update via Netlink (Remove IP)
        else Server Online
            Backend-->>CppDaemon: Connection Accepted
        end
    end

    Client->>Kernel: TCP SYN (HTTP Request)
    Kernel->>Kernel: Intercept via NF_INET_PRE_ROUTING
    Kernel->>Kernel: Read Healthy IPs List & Perform DNAT
    Kernel->>Backend: Forward Modified Packet
    Backend-->>Client: TCP SYN-ACK (Direct Server Return)
