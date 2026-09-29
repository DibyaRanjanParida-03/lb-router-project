# Backend Node State Machine
mermaid
stateDiagram-v2
[*] --> ONLINE : Node Added
ONLINE --> DEGRADED : TCP Check Failed (1st Time)
    DEGRADED --> OFFLINE : TCP Check Failed (2nd Time)
    DEGRADED --> ONLINE : TCP Check Success
    
    OFFLINE --> RECOVERING : TCP Check Success (1st Time)
    RECOVERING --> ONLINE : TCP Check Success (2nd Time)
    RECOVERING --> OFFLINE : TCP Check Failed
    
    OFFLINE --> [*] : Node Removed Manually
