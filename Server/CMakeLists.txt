project(Server)
cmake_minimum_required(VERSION 3.14)

include_directories(../dependencies/curl-7.74.0/include)
include_directories(../SharedRessources/)
include_directories(../dependencies/libmicrohttpd-0.9.72-w32-bin/x86_64/MinGW/static/mingw64/include/)
link_directories(../dependencies/libmicrohttpd-0.9.72-w32-bin/x86_64/MinGW/static/mingw64/lib/)

set(SERVER_SRC main.cpp storagedirectory.cpp storagedirectory.h commandocentral.cpp commandocentral.h)

add_executable(server WIN32 ${SERVER_SRC})

target_link_libraries(server microhttpd seadoggo_shared)