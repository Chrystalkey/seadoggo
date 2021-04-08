//
// Created by Chrystalkey on 26.02.2021.
//

#ifndef SEADOGGO_COMMANDRECEIVER_H
#define SEADOGGO_COMMANDRECEIVER_H

#include <string>
#include <filesystem>
#include "SDHelpers.h"
#include <curl/curl.h>

class CommandReceiver {
public:
    explicit CommandReceiver(const std::string &host, const std::string &userpwd, uint16_t port = SEADOGGO_CLIENT_PORT);

    static void finish() {
        if (onlyReceiver)
            delete onlyReceiver;
    }

    ~CommandReceiver();

    std::vector<std::string> listNSend(const fs::path &p);

    bool sendFile(const fs::path &p);

private:
    static size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data) {
        data->append((char *) ptr, size * nmemb);
        return size * nmemb;
    }

    static size_t readFunction(char *buffer, size_t size, size_t nmemb, std::ifstream *userdata);

    uint16_t port;
    std::string host;
    std::string userpwd;
    boolean listsent = false;
    static CommandReceiver *onlyReceiver;
};


#endif //SEADOGGO_COMMANDRECEIVER_H
