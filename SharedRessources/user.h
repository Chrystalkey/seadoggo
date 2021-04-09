//
// Created by Chrystalkey on 04.02.2021.
//

#ifndef SEADOGGO_USER_H
#define SEADOGGO_USER_H


#include <string>
#include <thread>
#include "Enumerations.h"


class Configuration;
namespace SeaDoggo{
class User {
public:
    enum Backupstate{
        BACKUP_NOTHREAD,
        BACKUP_INIT,
        BACKUP_FILELISTING,
        BACKUP_FILEMOVE,
        BACKUP_FILEREQUESTWAIT,
        BACKUP_FILEREQUESTCOMPLETE,
        BACKUP_FILEUPDATE,
        BACKUP_DONE,
        BACKUP_CANCELLED,
        BACKUP_ERROR
    };
    explicit User(const std::wstring &name, const std::string &pwdHash):
            name(name),pwdHash(pwdHash){}
    explicit User(){}

    void initCore(const User &other){
        name = other.name;
        pwdHash = other.pwdHash;
    }
public:
    void setName(const std::wstring &name) {
        User::name = name;
    }
    void setClient(uint16_t client_ref){

    }
    uint16_t getClient()const{return cl_ref;}
    void resetClient(){cl_ref=-1;}

    const std::wstring &getName()const {
        return name;
    }
    const std::string &getPwdHash() const {
        return pwdHash;
    }

    void clear(){
        name.clear();
        pwdHash.clear();
    }

    bool isValid() const{
        return !name.empty() && !pwdHash.empty();
    }

    bool cancelOps() {
        if(!hasThread()) return false;

        cancel_operations = true;
        stage = BACKUP_CANCELLED;
        return true;
    }

    template<typename _Tp>
    using not_same = std::__not_<std::is_same<std::__remove_cvref_t<_Tp>, std::thread>>;
    template<typename _Callable, typename... _Args,
    typename = std::_Require<not_same<_Callable>>>
    void assignThread(_Callable&& f, _Args&&... args){
        cancel_operations = false;
        stage = BACKUP_INIT;
        storage_thread = new std::thread(f, args...);
    }
    bool hasThread(){return storage_thread!=nullptr;}
    Backupstate getRunnerStage()const{return stage;}
    void setStage(Backupstate new_stage){ stage=new_stage;}
private:
    std::wstring name;  // username
    std::string pwdHash;   // hashed password
    std::thread *storage_thread = nullptr;
    bool cancel_operations = false;
    Backupstate stage = BACKUP_NOTHREAD;
    uint16_t cl_ref = -1;
    friend class ::Configuration;
};
}

#endif //SEADOGGO_USER_H
