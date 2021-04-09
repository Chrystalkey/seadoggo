//
// Created by Chrystalkey on 23.01.2021.
//

#ifndef SEADOGGO_JSONPARSER_H
#define SEADOGGO_JSONPARSER_H

#include <fstream>
#include <filesystem>
#include <ostream>

#include "../dependencies/cJSON-1.7.14/cJSON.h"
#include "SDHelpers.h"
#include "fsmodel.h"

using DiffStates = SeaDoggo::DiffStates;

class JSONTypeException : std::runtime_error{
public:
    explicit JSONTypeException(const std::string &blob) : std::runtime_error(blob) {}
};

class JSONParserDefault{
public:
    operator std::string(){throw JSONTypeException("INVALID FUNCTION CALL.");}
};


class JSONParser {
public: // JSONParser REALLY is only a collection of static stuff.
    explicit JSONParser() = delete;
    JSONParser(const JSONParser &other) = delete;
    JSONParser operator=(const JSONParser &) = delete;
    JSONParser operator=(JSONParser &) = delete;
    JSONParser operator=(const JSONParser &&)= delete;
    JSONParser operator=(JSONParser &&)= delete;
public: // parse it to strings, OHHH YEAH BABY!!
    static std::string parseToJSON(const DiffStateMap& dsm);
    static std::string parseToJSON(const FSEntityStateMap& esm);
    static std::string parseToJSON(const FSTree& tree);
    static std::string parseToJSON(const FSEntityVector &vec);

    template<typename T>
    static std::string parseToJSON(const T &);
private: // helper for the stringification
    static cJSON *cj_parseToJSON(const fs::path &p){return cj_parseToJSON(p.string());}
    static cJSON *cj_parseToJSON(const std::string &str){return cJSON_CreateString(str.c_str());}
    static cJSON *cj_parseToJSON_rec(const sptr<const FSDir> &entity);
    template<class K, class V>
    static cJSON *cj_parseToJSON(const std::map<K,V> mapping);
    template<class T>
    static cJSON *cj_parseToJSON(const std::vector<T> mapping);
    template<class T>
    static cJSON *cj_parseToJSON(const std::list<T> list);
    static cJSON *cj_parseToJSON(const sptr<const FSEntity> &entity);
    static cJSON *cj_parseToJSON(const DiffStates state);

public:// parse it to Data Structures
    static FSTree readInTree(const std::wstring& json);
    static FSEntityVector readInVector(const std::wstring &json);
    static DiffStateMap readInDSM(const std::wstring &json);
    static FSEntityStateMap readInESM(const std::wstring &json);
private:
    static sptr<FSEntity> cj_readInFSEntity(cJSON *json);
private:
    static std::pair<std::string, cJSON*> parseHead(const std::wstring& json);
};
#endif //SEADOGGO_JSONPARSER_H
