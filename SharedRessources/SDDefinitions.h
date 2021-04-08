//
// Created by Chrystalkey on 08.04.2021.
//

#ifndef SEADOGGO_SDDEFINITIONS_H
#define SEADOGGO_SDDEFINITIONS_H

#define SEADOGGO_STRINGIFY(x) #x

#define SEADOGGO_CLIENT_PORT 457
#define SEADOGGO_SERVER_PORT 458
#define RECEIVE_BUFLEN 512

#define SEADOGGO_CLIENT_PORT_STR SEADOGGO_STRINGIFY(SEADOGGO_CLIENT_PORT)
#define SEADOGGO_SERVER_PORT_STR SEADOGGO_STRINGIFY(SEADOGGO_SERVER_PORT)

#define SEADOGGO_CLIENT_UNDEFINED 0
#define SEADOGGO_CLIENT_INVALID -1

namespace fs = std::filesystem;
typedef uint16_t HttpResponseCode;
typedef uint16_t ClientHandle;

template<typename T>
using uptr = std::unique_ptr<T>;
template<typename T>
using sptr = std::shared_ptr<T>;
template<typename T>
using wptr = std::weak_ptr<T>;

namespace fs = std::filesystem;
#endif //SEADOGGO_SDDEFINITIONS_H
