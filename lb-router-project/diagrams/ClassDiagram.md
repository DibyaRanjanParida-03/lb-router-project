# C++ Daemon Class Diagram
mermaid
classDiagram
class LoadBalancerDaemon {
+start()
+stop()
}
class ServerPool {
-std::vector~std::shared_ptr~BackendNode~~ nodes
+addNode(ip, port)
+removeNode(ip)
+getHealthyNodes()
}
class HealthMonitor {
-ServerPool& pool
-NetlinkBridge& bridge
+startAsyncChecks()
-tcpPing(ip, port) bool
}
class NetlinkBridge {
-int socket_fd
+sendUpdateToKernel(ip, status)
}
class BackendNode {
-std::string ip_address
-uint16_t port
-bool is_healthy
+updateStatus(bool)
}
LoadBalancerDaemon --> ServerPool
    LoadBalancerDaemon --> HealthMonitor
    LoadBalancerDaemon --> NetlinkBridge
    HealthMonitor --> ServerPool
    HealthMonitor --> NetlinkBridge
    ServerPool *-- BackendNode
