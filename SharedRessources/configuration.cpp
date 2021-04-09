//
// Created by Chrystalkey on 21.01.2021.
//

#include "configuration.h"
#include <fstream>
#include "SDHelpers.h"

std::wstring toUpperCase(const std::wstring &randomstuff){
    std::wstring newOne;
    for(wchar_t c: randomstuff){
        newOne.push_back(std::toupper<wchar_t>(c, std::locale("utf-8")));
    }
    return newOne;
}

Configuration::Configuration(const fs::path &path)
    : configuration(std::make_shared<std::unordered_map<std::string, SeaDoggo::User>>()) {
    std::wifstream file(path);
    if(!file.is_open()){
        perror("Error open");
        reset_configuration();
        logLevel = SeaDoggo::LogLevel::Error;
        return;
    }
    std::vector<std::wstring> lines(file.gcount()/20);
    std::wstring line;

    SeaDoggo::User selected_user;
    std::vector<std::wstring> lineSplit;

    while(std::getline(file,line)){
        if(line.starts_with(L'#')||line.starts_with(L"//")) // komplett auskommentierte Zeilen Skippen.
            continue;

        //remove comments
        line = line.substr(0,line.find_first_of(L'#'));
        lineSplit = split(line, std::wregex(L"\\s+"));

        if(selected_user.getName().empty()) // no user is selected, everything is global, afterwards search until you find one
            if(pre_user_config(line))
                continue;

        if(user_config_one(line, &selected_user))
            continue;

        if(lineSplit.size()<2)
            continue;
        if(user_config_two(lineSplit,&selected_user))
            continue;
    }
    lines.shrink_to_fit();
}

void Configuration::reset_configuration() {
    logLevel = SeaDoggo::LogLevel::Debug;
    configuration->clear();
}

bool Configuration::isValidUserConfig(const SeaDoggo::User &config) {
    return config.isValid();
}

void Configuration::resetUserConfig(SeaDoggo::User *config) {
    config->clear();
}

bool Configuration::pre_user_config(const std::wstring& argument) {
    if(toUpperCase(argument).starts_with(L"LOGLEVEL")){
        if(logLevel != SeaDoggo::LogLevel::NotSet)
            return true;
        std::wstring decider = toUpperCase(argument.substr(argument.find_first_of(L' ')));

        /*if(decider.starts_with(L"DEBUG"))
        //    logLevel = SeaDoggo::LogLevel::Debug;
        else*/
        if(decider.starts_with(L"INFO"))
            logLevel = SeaDoggo::LogLevel::Info;
        else if(decider.starts_with(L"WARNING"))
            logLevel = SeaDoggo::LogLevel::Warning;
        else if(decider.starts_with(L"ERROR"))
            logLevel = SeaDoggo::LogLevel::Error;
        else
            logLevel = SeaDoggo::LogLevel::Debug;

        return true;
    }
    return false;
}

bool Configuration::user_config_one(const std::wstring &argument, SeaDoggo::User *current_config) {
    if(argument.starts_with(L'[')){ // a new user
        if(current_config->isValid())
            configuration->emplace(toString(current_config->getName()), *current_config);
        current_config->clear();

        std::wstring temporaryUsername;
        int pos = argument.find(L'['); // find the closing bracket
        temporaryUsername = argument.substr(pos+1,argument.find(L']'));
        if(temporaryUsername.find(L'[')!=-1||temporaryUsername.find(L']')!=-1)
            return false;
        current_config->setName(temporaryUsername);
        return true;
    }
    return false;
}

bool Configuration::user_config_two(const std::vector<std::wstring> &arguments, SeaDoggo::User *current_config) {
    if(toUpperCase(arguments[0]).starts_with(L"LOGINTYPE")){
        std::wstring second = toUpperCase(arguments[1]);
        return true;
    }

    if(toUpperCase(arguments[0]).starts_with(L"PASS")){
        current_config->pwdHash = (toString(arguments[1]));
        return true;
    }
    return false;
}
