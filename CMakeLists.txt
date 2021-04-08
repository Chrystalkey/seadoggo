cmake_minimum_required(VERSION 3.17)
project(seadoggo)

set(CMAKE_CXX_STANDARD 20)


set(CURL_DISABLE_LDAP ON)
set(CURL_DISABLE_IMAP ON)
set(CURL_DISABLE_POP3 ON)
set(CURL_DISABLE_MQTT ON)
set(CURL_DISABLE_RTSP ON)
set(CURL_DISABLE_TELNET ON)
set(CURL_DISABLE_SMTP ON)
set(CURL_DISABLE_GOPHER ON)
set(CURL_DISABLE_DICT ON)
set(CURL_DISABLE_FILE ON)
set(CURL_DISABLE_FTP ON)
set(CURL_DISABLE_SMB ON)
set(CURL_DISABLE_TFTP ON)

set(CMAKE_USE_OPENSSL ON)
set(BUILD_SHARED_LIBS OFF)
option(BUILD_SHARED_LIBS OFF)


add_subdirectory(dependencies/curl-7.74.0/)
#add_subdirectory(dependencies/libmicrohttpd-0.9.72/src/)
add_subdirectory(dependencies/cJSON-1.7.14/)

add_subdirectory(Server)
add_subdirectory(SharedRessources)
add_subdirectory(Client)