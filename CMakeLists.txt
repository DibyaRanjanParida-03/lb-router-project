cmake_minimum_required(VERSION 3.10)
project(LB_Router_Daemon VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

include_directories(include)

add_executable(lb_daemon 
    src/main.cpp 
    src/NetlinkBridge.cpp
)

target_link_libraries(lb_daemon Threads::Threads)
