//
// Created by Chrystalkey on 26.02.2021.
//

#include "commandreceiver.h"
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

CommandReceiver *CommandReceiver::onlyReceiver = nullptr;

CommandReceiver::CommandReceiver(const std::string &host, const std::string &userpwd, uint16_t port) : host(host),
                                                                                                       userpwd(userpwd),
                                                                                                       port(port) {
    if (onlyReceiver != nullptr)
        throw std::runtime_error("Only one Instance allowed");
    curl_global_init(CURL_GLOBAL_ALL);
    CommandReceiver::onlyReceiver = this;
}

CommandReceiver::~CommandReceiver() {
    curl_global_cleanup();
}

std::vector<std::string> CommandReceiver::listNSend(const fs::path &p) {
    CURL *curl;
    curl = curl_easy_init();
    std::vector<fs::path> input;
    for (auto &path : fs::recursive_directory_iterator(p)) {
        input.push_back(path);
    }
    std::string result_string;
    std::ranges::sort(input);
    std::for_each(input.begin(), input.end(), [&](const fs::path &element) {
        result_string += element.string() + "\n";
    });
    result_string = curl_easy_escape(curl, result_string.c_str(), result_string.length());

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_URL, host.c_str());
    curl_easy_setopt(curl, CURLOPT_PORT, port);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ("method=LIST&listing=" + result_string).c_str());

    std::string body;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    std::string header;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);

    long resp_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return {};
    }
    int l;
    std::vector<std::string> blob = split(
            std::string(curl_easy_unescape(curl, body.c_str(), body.length(), &l), l), std::regex(R"(\n)")
    );

    curl_easy_cleanup(curl);
    listsent = true;
    return blob;
}

bool CommandReceiver::sendFile(const fs::path &p) {
    if (listsent == false)// if no listing was sent beforehand, no files should be sent either
        return false;

    CURL *curl = curl_easy_init();
    std::ifstream in(p);
    if (!in.is_open()) {
        std::cerr << "Error opening file " << p << std::endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_URL, host.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readFunction);
    curl_easy_setopt(curl, CURLOPT_READDATA, &in);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error performing file upload: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);
    return true;
}

size_t CommandReceiver::readFunction(char *buffer, size_t size, size_t nmemb, std::ifstream *userdata) {
    auto x = userdata->tellg();
    userdata->read(buffer, size);
    return userdata->tellg() - x;
}
