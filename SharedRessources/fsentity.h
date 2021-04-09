//
// Created by Chrystalkey on 12.11.2020.
//

#ifndef SEADOGGO_FSENTITY_H
#define SEADOGGO_FSENTITY_H

#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <list>
#include "SDDefinitions.h"

class FSFile;

class FSDir;

class FSEntity {
public:
    sptr<FSDir> parent() const { return _parent; }

    virtual ~FSEntity() {}

    fs::path path() const { return _path; }

    void setFound() { found = true; }

    void resetFound() { found = false; }

    bool isFound() const { return found; }

    virtual bool isDir() const = 0;

    bool isFile() const { return !isDir(); }

protected:
    explicit FSEntity(const std::wstring &path, const sptr<FSDir> &parent = nullptr) : _parent(parent), _path(path) {}

private:
    bool found = false;
    sptr<FSDir> _parent;
    fs::path _path;
};

class FSFile : public FSEntity {
public:
    explicit FSFile(const std::wstring &path, const fs::file_time_type mtime, const sptr<FSDir> &parent = nullptr) :
            FSEntity(path, parent), _mtime(mtime) {}

    ~FSFile() {}

    [[nodiscard]] const std::string &getHash() const { return _hash; }

    [[nodiscard]] const fs::file_time_type &getMtime() const { return _mtime; }

    void setHash(const std::string &hash) { _hash = hash; }

    void setMtime(const fs::file_time_type &mtime) { _mtime = mtime; }

    [[nodiscard]] bool isDir() const override { return false; };

private:
    std::string _hash;
    fs::file_time_type _mtime;

    friend class FSModel;
};

class FSDir : public FSEntity {
public:
    explicit FSDir(const std::wstring &path, const sptr<FSDir> &parent = nullptr) :
            FSEntity(path, parent) {}

    ~FSDir() {}

    void addChild(const sptr<FSDir> &child) { child_directories.push_back(child); }

    void addChild(const sptr<FSFile> &child) { _files.push_back(child); }

    const std::vector<sptr<FSDir>> &getChildDirectories() const { return child_directories; }

    const std::vector<sptr<FSFile>> &getFiles() const { return _files; }

    std::size_t recFileCount() const { return child_files; }

    std::size_t recDirCount() const { return child_dirs; }

    bool isDir() const override { return true; };

    //static const FSEntityVector traverseFilesFirst(const FSDir* root);
    //static const FSEntityVector traverseDirsFirst(const FSDir* root);

private:
    //static std::list<const FSEntity*> internalTravFilesFirst(const FSDir* root);
    //static std::list<const FSEntity*> internalTravDirsFirst(const FSDir* root);

private:
    std::vector<sptr<FSDir>> child_directories;
    std::vector<sptr<FSFile>> _files;
    std::size_t child_files = 0;
    std::size_t child_dirs = 0;

    friend class FSModel;
};

#endif //SEADOGGO_FSENTITY_H
