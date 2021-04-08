//
// Created by Chrystalkey on 12.02.2021.
//

#ifndef SEADOGGO_WEBDAVBUILDER_H
#define SEADOGGO_WEBDAVBUILDER_H

#include <filesystem>
#include <curl/curl.h>

/**
 * Builds the appropiate CURL states to call after with the information provided.
 */
class WebDavBuilder {
public:
    explicit WebDavBuilder(const std::string &_host, const std::string &_user, const std::string &_api_key = "");
    ~WebDavBuilder();
    static CURL* initCurl();

    static CURL* request_content_list(CURL* curl, const fs::path &folder);
    static CURL* request_file_mtime(CURL* curl, const fs::path &file);
    static CURL* post_newFile(CURL* curl, const fs::path);
private:
    static request_preparation(CURL *curl);
private:
    static std::string host;
    static std::string user;
    static std::string api_key;
};


#endif //SEADOGGO_WEBDAVBUILDER_H
