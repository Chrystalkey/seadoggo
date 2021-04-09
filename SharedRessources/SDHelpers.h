//
// Created by Chrystalkey on 21.01.2021.
//

#ifndef SEADOGGO_SDHELPERS_H
#define SEADOGGO_SDHELPERS_H

#include <regex>
#include <codecvt>
#include <chrono>
#include <filesystem>
#include "SDDefinitions.h"


std::vector<std::wstring> split(const std::wstring &string, const std::wregex &regex);
std::vector<std::string> split(const std::string &string, const std::regex &regex);

static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

std::string toString(const std::wstring &origin);
std::wstring toWString(const std::string &origin);

std::string currentISO8601TimeUTC();
std::string toISOTime(std::chrono::system_clock::time_point time);
std::string toISOTime(fs::file_time_type time);
std::time_t fromISOTime(const std::string &);

std::string now_time();

class one_inst_error : public std::runtime_error{
public:
    one_inst_error(const std::string &arg);
};

#endif //SEADOGGO_SDHELPERS_H
