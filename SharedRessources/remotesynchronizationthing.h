//
// Created by Chrystalkey on 21.01.2021.
//

#ifndef SEADOGGO_REMOTESYNCHRONIZATIONTHING_H
#define SEADOGGO_REMOTESYNCHRONIZATIONTHING_H

#include "configuration.h"
#include "fsmodel.h"

class RemoteSynchronizationThing {
public:
    explicit RemoteSynchronizationThing(SeaDoggo::LoginType ltype);

    virtual ~RemoteSynchronizationThing() = 0;

    virtual bool synchronize(const DiffStateMap &dsm) = 0;
    virtual bool synchronize(const FSEntityStateMap &dsm) {return synchronize(FSModel::translateToDSM(dsm));}

    SeaDoggo::LoginType getLType() const { return ltype; }

protected:
    const SeaDoggo::LoginType ltype;
};

class NetworkSynchro : public RemoteSynchronizationThing {
public:
    explicit NetworkSynchro(SeaDoggo::LoginType lt,
                            const std::string &host,
                            const std::wstring &username,
                            const std::string &password);

    const std::string &getHost() const { return host; }

    const std::wstring &getUsername() const { return username; }

    const std::string &getPassword() const { return password; }

protected:
    const std::string host;
    const std::wstring username;
    const std::string password;
};

class SFTPSynchro : public NetworkSynchro {
public:
    explicit SFTPSynchro(const std::string &host,
                         const std::wstring &username,
                         const std::string &password);
    bool synchronize(const DiffStateMap &dsm) override{return false;}
    ~SFTPSynchro() override = default;
};

class SambaSynchro : public NetworkSynchro {
public:
    explicit SambaSynchro(const std::string &host,
                          const std::wstring &username,
                          const std::string &password);

    ~SambaSynchro() override = default;
    bool synchronize(const DiffStateMap &dsm) override{return false;}

};

class FSysSynchro : public RemoteSynchronizationThing {
public:
    explicit FSysSynchro(const fs::path &path);

    ~FSysSynchro() override = default;
    bool synchronize(const DiffStateMap &dsm) override{return false;}
private:
    const fs::path path;
};

#endif //SEADOGGO_REMOTESYNCHRONIZATIONTHING_H
