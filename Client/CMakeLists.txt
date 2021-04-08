project(Client)

include_directories(../SharedRessources/)
set(source
            main.cpp
        commandreceiver.cpp commandreceiver.h)

add_executable(client ${source})

target_link_libraries(client libcurl)
target_link_libraries(client seadoggo_shared)
