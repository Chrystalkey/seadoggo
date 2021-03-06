//
// Created by Chrystalkey on 13.02.2021.
//

#ifndef SEADOGGO_STORAGEDIRECTORY_H
#define SEADOGGO_STORAGEDIRECTORY_H

#include <filesystem>
#include <thread>

#include "user.h"
#include "fsmodel.h"
#include "configuration.h"

class CommandoCentral;

class StorageDirectory {
public:
    explicit StorageDirectory(CommandoCentral *parent, fs::path baseDir = "dummy/");

    /**
     * for step 1 of the protocol. if the user with given usertag already has a backup, restore his settings into a
     * "User" class. if not, fuck it and return 0.
     * @param usertag the usertag
     * @return if user is configured, return the user configuration class, if not return 0
     */

    SeaDoggo::User* request_user(const std::string &usertag) {
        if (users->contains(usertag) && !users->at(usertag).hasThread())
            return &users->at(usertag);
        else
            return nullptr;
    }

    /**
     * 1-, build entity -tree and perform difflisting on user-assigned thread - if new client wants to perform backup for the same user, ret nullptr
     * 2-, write difflist to logdir
     * 3-, return filelist with new files back to central control
     * 4-, wait for files to dripple in, confirm every one with the server to get it out of the Set in ClientReference
     * 5-, if files are complete, move files into incremental storage, because now it cannot be stopped.
     *
     * profit.
     *
     * @param user the user with corresponding usertag for which the given operation should be done
     * @param filelist a list with files present on the remote side, maybe i can json that stuff a little up
     */
    bool new_backup_request(SeaDoggo::User *user, const std::string &filelist);

    /**
     * stop dripple-in-waitingloop, new_backup_request_loop, when the transaction is marked as moving into
     * incremental storage, you cannot break it and this operation will fail.
     * @param user, obviously.
     */
    bool cancel_operations(SeaDoggo::User *user) { return user->cancelOps(); }

    /**
     * create subdirectories and directories as well as configuration files for new user "User"
     * @param user the user to be put in place
     */
    void add_new_user(const SeaDoggo::User *user);

    /**
     * figures out whether user "User" has a directory and valid subdirectories
     * @param user the user to be checked
     * @return true gdw everything is correct, false otherwise.
     */
    bool user_exists(const SeaDoggo::User *user);

    /**
     * replaces file at relative path re_path with the file at input_stream
     * @param username the user to which all of this belongs
     * @param re_path the relative path of the file to be replaced
     * @param input_stream the new file data
     */
    void updateFile(const SeaDoggo::User &username, const fs::path &re_path, std::istream *input_stream);

    /**
     * removes user from the system permanently, but not from the config files.
     * warning: can cause some problems, only use when debugging or if you know what you're doing
     * @param user the user to be removed
     */
    void delete_user_directory(const SeaDoggo::User &user);

    /**
     * resolves differences by
     * @param user
     * @param differences
     */
    void resolve_differences(const SeaDoggo::User &user, const DiffStateMap &differences);

    /**
     * for step 2-4
     * @param user the user for which to calculate the differences
     * @param client_tree the clients submitted tree
     */
    void find_differences(const SeaDoggo::User user, const FSTree &client_tree);

    void recv_file(const SeaDoggo::User *user, sptr<std::ifstream> input, const fs::path &path);

private:
    void exchange_file(const SeaDoggo::User *user, const fs::path &saved, sptr<std::ifstream> input);
    CommandoCentral *parent;
    Configuration cfg;

    void log_entry(const std::string &entry);
    fs::path base_dir;
    sptr<std::unordered_map<std::string, SeaDoggo::User>> users;
};

#endif //SEADOGGO_STORAGEDIRECTORY_H
