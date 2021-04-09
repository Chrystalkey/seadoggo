//
// Created by Chrystalkey on 21.01.2021.
//

#ifndef SEADOGGO_CONFIGURATION_H
#define SEADOGGO_CONFIGURATION_H
#include <iostream>
#include <filesystem>
#include <map>
#include "Enumerations.h"
#include "user.h"
#include "SDDefinitions.h"


#define HASH_BYTE_SIZE 32

class Configuration {
public:
    explicit Configuration(const fs::path &path);

    wptr<std::unordered_map<std::string, SeaDoggo::User>> getConfiguration(){
        return configuration;
    };
    const sptr<std::unordered_map<std::string, SeaDoggo::User>> getConfiguration() const{
        return configuration;
    }
    SeaDoggo::LogLevel getLogLevel() const{
        return logLevel;
    }

    void reset_configuration();

private:
    SeaDoggo::LogLevel logLevel;
    sptr<std::unordered_map<std::string, SeaDoggo::User>> configuration;

    static bool isValidUserConfig(const SeaDoggo::User &config);
    static void resetUserConfig(SeaDoggo::User *config);

    bool pre_user_config(const std::wstring& argument);
    bool user_config_one(const std::wstring &argument, SeaDoggo::User *current_config);
    bool user_config_two(const std::vector<std::wstring> &arguments, SeaDoggo::User *current_config);
};

#endif //SEADOGGO_CONFIGURATION_H
