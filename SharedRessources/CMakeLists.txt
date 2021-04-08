project(SeadoggoShared)

set(SHARED_SRC configuration.cpp
                fsmodel.cpp
                jsonparser.cpp
                SDHelpers.cpp user.cpp user.h Enumerations.h FSStorageStructures.h SDDefinitions.h)

include_directories(../dependencies/cJSON-1.7.14)

add_library(seadoggo_shared STATIC ${SHARED_SRC})
target_link_libraries(seadoggo_shared libcurl cjson)