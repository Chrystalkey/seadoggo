//
// Created by Chrystalkey on 12.02.2021.
//

#include "webdavbuilder.h"

CURL *WebDavBuilder::initCurl() {
    curl_global_init(CURL_GLOBAL_SSL);
    CURL *curl = curl_easy_init();
    return curl;
}

WebDavBuilder::~WebDavBuilder() {
    curl_global_cleanup();
}

WebDavBuilder::WebDavBuilder(const std::string &_host, const std::string &_user, const std::string &_api_key) {
    WebDavBuilder::host = _host;
    WebDavBuilder::user = _user;
    WebDavBuilder::api_key = _api_key;
}

CURL *WebDavBuilder::request_content_list(CURL *curl, const fs::path &folder) {
    curl = request_preparation(curl);
    curl_easy_setopt(curl,CURLOPT_URL,(host+"/"+folder.string()).c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

}

CURL *WebDavBuilder::request_preparation(CURL *curl) {
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_USERPWD,(user+api_key).c_str());
    return curl;
}
