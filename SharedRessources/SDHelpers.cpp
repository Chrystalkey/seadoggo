//
// Created by Chrystalkey on 21.01.2021.
//

#include "SDHelpers.h"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <filesystem>

std::vector<std::wstring> split(const std::wstring &string, const std::wregex &regex) {
    std::vector<std::wstring> elements;

    std::wsregex_token_iterator t_iter(string.begin(), string.end(), regex,-1);
    std::wsregex_token_iterator end;

    while(t_iter != end){
        elements.push_back(*t_iter);
        ++t_iter;
    }

    return elements;
}

std::vector<std::string> split(const std::string &string, const std::regex &regex) {
    std::vector<std::string> elements;

    std::sregex_token_iterator t_iter(string.begin(), string.end(), regex,-1);
    std::sregex_token_iterator end;

    while(t_iter != end){
        elements.push_back(*t_iter);
        ++t_iter;
    }
    return elements;
}

std::string toString(const std::wstring &origin) {
    return converter.to_bytes(origin);
}

std::wstring toWString(const std::string &origin) {
    return converter.from_bytes(origin);
}

std::string now_time() {
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));
    return std::string(buf, sizeof buf);
}

std::string currentISO8601TimeUTC() {
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);

    std::ostringstream ss;
    ss << std::put_time(gmtime(&itt), "%F %T %Z");
    return ss.str();
}
std::string toISOTime(std::chrono::system_clock::time_point time) {
    auto itt = std::chrono::system_clock::to_time_t(time);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&itt));
    return std::string(buf, sizeof buf);
}

std::string toISOTime(std::filesystem::file_time_type time) {
    return toISOTime(std::chrono::file_clock::to_sys(time));
}

std::time_t fromISOTime(const std::string &timestring) {
    std::istringstream iss(timestring);
    struct std::tm tm;
    iss >> std::get_time(&tm, "%F %T %Z");
    return std::mktime(&tm);
}
