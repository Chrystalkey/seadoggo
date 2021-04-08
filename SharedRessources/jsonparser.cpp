//
// Created by Chrystalkey on 23.01.2021.
//

#include "jsonparser.h"
#include <iostream>

#include "fsmodel.h"

std::pair<std::string, cJSON *> JSONParser::parseHead(const std::wstring &json) {
    cJSON *cjson = cJSON_Parse(toString(json).c_str());
    if (cjson == nullptr)
        throw JSONTypeException("Could not parse given JSON");
    cJSON *typeJ = cJSON_GetObjectItem(cjson, "type");
    cJSON *contentJ = cJSON_GetObjectItem(cjson, "content");

    return {cJSON_GetStringValue(typeJ), contentJ};
}

std::string JSONParser::parseToJSON(const DiffStateMap &dsm) {
    cJSON *master = cj_parseToJSON(dsm);
    std::string krams = cJSON_Print(master);
    cJSON_Delete(master);
    return krams;
}

std::string JSONParser::parseToJSON(const FSEntityStateMap &esm) {
    return parseToJSON(FSModel::translateMap(esm));
}

std::string JSONParser::parseToJSON(const FSEntityVector &type){
    cJSON *master = cj_parseToJSON(type);
    std::string blob = cJSON_Print(master);
    cJSON_Delete(master);
    return blob;
}
std::string JSONParser::parseToJSON(const FSTree &tree){
    cJSON *master = cj_parseToJSON_rec(tree);
    std::string blob = cJSON_Print(master);
    cJSON_Delete(master);
    return blob;
}

cJSON *JSONParser::cj_parseToJSON_rec(const sptr<const FSDir> &entity) {
    cJSON *parent = cJSON_CreateObject();
    cJSON_AddStringToObject(parent,"path", entity->path().string().c_str());
    cJSON_AddStringToObject(parent, "type", "d");

    cJSON *files = cJSON_AddArrayToObject(parent, "files");
    cJSON *dirs = cJSON_AddArrayToObject(parent, "directories");

    for(auto &f : entity->getFiles()){
        cJSON *file = cJSON_CreateObject();
        cJSON_AddStringToObject(file, "path", f->path().string().c_str());
        cJSON_AddStringToObject(file, "type", "f");
        cJSON_AddStringToObject(file, "mtime", toISOTime(f->getMtime()).c_str());
        cJSON_AddItemToArray(files, file);
    }

    for(auto &d : entity->getChildDirectories()){
        cJSON_AddItemToArray(dirs, cj_parseToJSON_rec(d));
    }
    return parent;
}

template<class K, class V>
cJSON *JSONParser::cj_parseToJSON(const std::map<K, V> mapping) {
    cJSON *master = cJSON_CreateObject();
    cJSON *array = cJSON_AddArrayToObject(master, "map");
    for(auto &pair : mapping){
        cJSON *entry = cJSON_CreateObject();
        cJSON_AddItemToObject(entry,"key", cj_parseToJSON(pair.first));
        cJSON_AddItemToObject(entry,"val", cj_parseToJSON(pair.second));
        cJSON_AddItemToArray(array, entry);
    }
    return master;
}

template<class T>
cJSON *JSONParser::cj_parseToJSON(const std::vector<T> mapping) {
    cJSON *master = cJSON_CreateObject();
    cJSON *array = cJSON_AddArrayToObject(master, "vector");
    for(auto &entry : mapping){
        cJSON_AddItemToArray(array, cj_parseToJSON(entry));
    }
    return master;
}

template<class T>
cJSON *JSONParser::cj_parseToJSON(const std::list<T> list) {
    cJSON *master = cJSON_CreateObject();
    cJSON *array = cJSON_AddArrayToObject(master, "vector");
    for(auto &entry : list){
        cJSON_AddItemToArray(array, cj_parseToJSON(entry));
    }
    return master;
}

cJSON *JSONParser::cj_parseToJSON(const sptr<const FSEntity> &entity) {
    cJSON *parent = cJSON_CreateObject();
    cJSON_AddStringToObject(parent,"path", entity->path().string().c_str());
    cJSON_AddStringToObject(parent, "type", entity->isDir()?"d":"f");
    if(entity->isFile())
        cJSON_AddStringToObject(parent, "mtime", toISOTime((reinterpret_cast<const sptr<FSFile>&>(entity))->getMtime()).c_str());
    return parent;
}

cJSON *JSONParser::cj_parseToJSON(const DiffStates state) {
    return cJSON_CreateString(toString(SeaDoggo::diffstate_strings.at(state)).c_str());
}

DiffStateMap JSONParser::readInDSM(const std::wstring &json) {
    DiffStateMap dsm;
    cJSON* master = cJSON_Parse(toString(json).c_str()); // parse the whole tree

    if (master == nullptr)
        throw JSONTypeException("Could not parse given JSON");
    if(!cJSON_HasObjectItem(master, "map"))
        throw JSONTypeException("ERROR in readInDSM: not an DSM type");

    // the map, which is an array of std::pair<DiffStates, FSEntity>
    cJSON *map = cJSON_GetObjectItem(master, "map");
    std::size_t arr_sz = cJSON_GetArraySize(map);
    for(int i = 0; i < arr_sz; i++){
        cJSON *proto = cJSON_GetArrayItem(map, i); // represents the pair in string
        cJSON *key = cJSON_GetObjectItem(proto, "key");
        cJSON *val = cJSON_GetObjectItem(proto, "val");
        DiffStates diffstate = DiffStates::DiffStateError;
        std::vector<sptr<const FSEntity>> entVector;

        // its the diffstate
        diffstate = SeaDoggo::string_diffstates.at(toWString(cJSON_GetStringValue(key)));

        // its the vector of fsentities
        cJSON *vect = cJSON_GetObjectItem(val, "vector");
        std::size_t internal_asz = cJSON_GetArraySize(vect);
        entVector.reserve(internal_asz);
        for(int j = 0; j < internal_asz; j++){
            entVector.push_back(cj_readInFSEntity(cJSON_GetArrayItem(vect, j)));
        }
        dsm[diffstate] = entVector;
    }
    cJSON_Delete(master);
    return dsm;
}

sptr<FSEntity> JSONParser::cj_readInFSEntity(cJSON *json) {
    cJSON *type = cJSON_GetObjectItem(json, "type");
    cJSON *path = cJSON_GetObjectItem(json, "path");
    cJSON *mtime = nullptr;
    if(std::string(cJSON_GetStringValue(type)) == "d"){
        return std::move(std::make_shared<FSDir>(toWString(cJSON_GetStringValue(path)),nullptr));
    }else if(std::string(cJSON_GetStringValue(type)) == "f"){
        mtime = cJSON_GetObjectItem(json, "mtime");
        return std::move(std::make_shared<FSFile>(toWString(cJSON_GetStringValue(path)),
                          std::chrono::file_clock::from_sys(
                                  std::chrono::system_clock::from_time_t(
                                          fromISOTime(cJSON_GetStringValue(mtime))
                                          ))));
    }else{
        throw JSONTypeException("Error parsing FSEntity. not a valid type given.");
    }
}

FSEntityStateMap JSONParser::readInESM(const std::wstring &json) {
    return FSModel::translateMap(readInDSM(json));
}

FSTree JSONParser::readInTree(const std::wstring &json) {
    return FSTree();
}

FSEntityVector JSONParser::readInVector(const std::wstring &json) {
    return FSEntityVector();
}
