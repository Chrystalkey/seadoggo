//
// Created by Chrystalkey on 21.01.2021.
//

#include "remotesynchronizationthing.h"

RemoteSynchronizationThing::~RemoteSynchronizationThing(){}

NetworkSynchro::NetworkSynchro(SeaDoggo::LoginType lt, const std::string &host, const std::wstring &username,
                               const std::string &password) : RemoteSynchronizationThing(lt), host(host), username(username), password(password) {}

SFTPSynchro::SFTPSynchro(const std::string &host, const std::wstring &username, const std::string &password):
            NetworkSynchro(SeaDoggo::LoginType::SFTP, host, username, password){}

SambaSynchro::SambaSynchro(const std::string &host, const std::wstring &username, const std::string &password):
            NetworkSynchro(SeaDoggo::LoginType::Samba, host, username, password){}

FSysSynchro::FSysSynchro(const fs::path &path) : RemoteSynchronizationThing(SeaDoggo::LoginType::FSys), path(path) {}

RemoteSynchronizationThing::RemoteSynchronizationThing(SeaDoggo::LoginType ltype) :
ltype(ltype){}
