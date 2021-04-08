//
// Created by Chrystalkey on 13.02.2021.
//

#include <fstream>
#include "storagedirectory.h"
#include "SDHelpers.h"
#include "commandocentral.h"

void StorageDirectory::log_entry(const std::string &entry) {
    std::ofstream logfile(base_dir.string()+("storage.log"),std::ios_base::out|std::ios_base::app);
    if(logfile.good()){
        logfile << entry << std::endl;
    }
}

void StorageDirectory::delete_user_directory(const SeaDoggo::User &user) {
    std::string username = toString(user.getName());
    log_entry("Deletion Instruction for user "+username);
    if(!users.contains(username)){
        log_entry("User cannot be removed: nonexistent");
        return;
    }

    fs::remove_all(username);
    log_entry("User directory removed");
    users.erase(username);
    log_entry("User entry removed");
    log_entry("User successfully removed");
}

void StorageDirectory::updateFile(const SeaDoggo::User &username, const fs::path &re_path,
                                  std::istream *input_stream) {
    log_entry("Updating/Creating file: "+re_path.string() + " for user " + toString(username.getName()));
    std::ofstream file_output((base_dir/username.getName()/"vault"/re_path).string(), std::ios_base::out|std::ios_base::trunc);
    file_output << input_stream;
    log_entry("File update successful");
}

void StorageDirectory::add_new_user(const SeaDoggo::User &user) {
    fs::create_directories(base_dir / user.getName() / "vault");
    fs::create_directories(base_dir / user.getName() / "logs");
    fs::create_directories(base_dir / user.getName() / "incremental_storage");
}

StorageDirectory::StorageDirectory(CommandoCentral *parent, const fs::path &baseDir) : parent(parent), base_dir(baseDir) {
    log_entry("New StorageDirectory session inited");
}

bool StorageDirectory::new_backup_request(SeaDoggo::User *user, const std::string &filelist) {
    if(filelist.empty()) {
        user->setStage(SeaDoggo::User::Backupstate::BACKUP_ERROR);
        return false;
    }
    std::function<void(CommandoCentral *, SeaDoggo::User*,std::string)> threadfunc = [](CommandoCentral* central, SeaDoggo::User* usr, const std::string &filelist){
        // first split into files
        usr->setStage(SeaDoggo::User::BACKUP_FILELISTING);
        // second, build file trees
        // third, with the user in mind, push the filelist into commandocentral

        std::unordered_set<std::string> requestfiles;
        central->set_requestfiles_for_client(usr->getClient(), std::move(requestfiles));
        // then do the other stuff. -_(* *)_-
    };
    user->assignThread(threadfunc, parent, user, filelist);
}
