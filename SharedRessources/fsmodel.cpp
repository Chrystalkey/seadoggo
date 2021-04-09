//
// Created by Chrystalkey on 12.11.2020.
//

#include "fsmodel.h"
#include "SDHelpers.h"
#include <map>

/*
const FSEntityVector FSDir::traverseFilesFirst(const FSDir *root) {
    auto x = internalTravFilesFirst(root);
    std::vector<const FSEntity*> v(x.begin(),x.end());
    return v;
}

const FSEntityVector FSDir::traverseDirsFirst(const FSDir *root) {
    auto x = internalTravDirsFirst(root);
    std::vector<const FSEntity*> v(x.begin(),x.end());
    return v;
}

std::list<const FSEntity *> FSDir::internalTravFilesFirst(const FSDir *root) {
    std::list<const FSEntity *> instance;
    instance.push_back(root);
    for(auto &t: root->getFiles()){
        instance.push_back(&t);
    }
    for(auto &t: root->getChildDirectories()){
        instance.push_back(&t);
        instance.splice(instance.end(), internalTravFilesFirst(&t));
    }
    return instance;
}

std::list<const FSEntity *> FSDir::internalTravDirsFirst(const FSDir *root) {
    std::list<const FSEntity *> instance;
    instance.push_back(root);
    for(auto &t: root->getChildDirectories()){
        instance.push_back(&t);
        instance.splice(instance.end(), internalTravFilesFirst(&t));
    }
    for(auto &t: root->getFiles()){
        instance.push_back(&t);
    }
    return instance;
}

*/

void FSModel::discoverFSStructure(const sptr<FSDir> &parent) {
    for (const auto &entry: fs::directory_iterator(parent->path())) {
        if (entry.is_directory()) {
            sptr<FSDir> entryDir = std::make_shared<FSDir>(entry.path(), parent);
            discoverFSStructure(entryDir);
            parent->addChild(entryDir);
            parent->child_dirs += entryDir->child_dirs;
            parent->child_files += entryDir->child_files;
            continue;
        }
        parent->addChild(std::make_shared<FSFile>(fs::path(entry).wstring(), entry.last_write_time(), parent));
    }
    parent->child_dirs += parent->child_directories.size();
    parent->child_files += parent->_files.size();
}

void FSModel::discoverFSStructure(const fs::path &root_dir) {
    root.reset();
    root = std::make_shared<FSDir>(root_dir, nullptr);
    discoverFSStructure(root);
}

void FSModel::createFSStructure(const std::string &source) {
    root.reset();
    //TODO: wtf do I want source to be? JSON? a list with line breaks? what???
}

void FSModel::insert_into_tree(const std::vector<std::string> &segments, sptr<FSDir> parent, const std::time_t *mtime,
                               std::size_t index) {
    if (index >= segments.size())
        return;
    if (index == segments.size() - 1 && mtime != nullptr) { // if its a file and we are at the last segment
        parent->addChild(
                std::make_shared<FSFile>(toWString(segments[index]), chrf::from_sys(chrs::from_time_t(*mtime)),
                                         parent));
        return;
    }
    auto child = std::make_shared<FSDir>(toWString(segments[index]),parent);
    parent->addChild(child);
    insert_into_tree(segments,child,mtime,index+1);
}

FSEntityStateMap FSModel::performDiff(FSModel &other) {
    FSEntityStateMap stateMap;
    if (other.root == nullptr || this->root == nullptr)
        return stateMap;
    _recursiveDifference(root, other.root, &stateMap,
                         root->path(), other.root->path());
    return stateMap;
}

//one => this; two => other
void FSModel::_recursiveDifference(sptr<FSDir> one, sptr<FSDir> two, FSEntityStateMap *differences,
                                   const fs::path &baseOne, const fs::path &baseTwo) {
    if (one == nullptr && two == nullptr)
        return;

    if (one == nullptr) {
        auto *twofiles = &(two->_files);
        auto *twodirs = &(two->child_directories);
        for (auto &file: *twofiles) {
            differences->emplace(file, SeaDoggo::DiffStates::fileMissingThis);
        }
        for (auto &dir: *twodirs) {
            differences->emplace(dir, SeaDoggo::DiffStates::dirMissingThis);
            _recursiveDifference(nullptr, dir, differences, baseOne, baseTwo);
        }
        return;
    }

    if (two == nullptr) {
        auto *onefiles = &(one->_files);
        auto *onedirs = &(one->child_directories);
        for (auto &file: *onefiles) {
            differences->emplace(file, SeaDoggo::DiffStates::fileMissingOther);
        }
        for (auto &dir: *onedirs) {
            differences->emplace(dir, SeaDoggo::DiffStates::dirMissingOther);
            _recursiveDifference(dir, nullptr, differences, baseOne, baseTwo);
        }
        return;
    }
    auto *onefiles = &(one->_files);
    auto *onedirs = &(one->child_directories);
    auto *twofiles = &(two->_files);
    auto *twodirs = &(two->child_directories);
    //for (auto otherit = twofiles->begin(); otherit != twofiles->end(); otherit++) {
    for (auto &twoElement : *twofiles) {
        bool matchFound = false;
        //for (auto thisit = onefiles->begin(); thisit != onefiles->end(); thisit++) {
        for (auto &oneElement : *onefiles) {
            if (twoElement->path().lexically_relative(baseTwo).compare(
                    oneElement->path().lexically_relative(baseOne)
            ) == 0) {
                oneElement->setFound();
                matchFound = true;
                if (oneElement->getMtime() < twoElement->getMtime()) { // thisfile is older
                    differences->emplace(oneElement, SeaDoggo::DiffStates::fileMtimeMoreRecentOther);
                } else if (oneElement->getMtime() > twoElement->getMtime()) {
                    differences->emplace(oneElement, SeaDoggo::DiffStates::fileMtimeMoreRecentThis);
                } else {
                    differences->emplace(oneElement, SeaDoggo::DiffStates::equal);
                }
            }
        }

        if (!matchFound) {
            differences->emplace(twoElement, SeaDoggo::DiffStates::fileMissingThis);
        }

    }

    //for (auto thisit = onefiles->begin(); thisit != onefiles->end(); thisit++) {
    for (auto &oneElement : *onefiles) {
        if (!oneElement->isFound())
            differences->emplace(oneElement, SeaDoggo::DiffStates::fileMissingOther);
        oneElement->resetFound();
    }

    // directories
    //for (auto otherit = twodirs->begin(); otherit != twodirs->end(); otherit++) {
    for (auto &twoElement : *twodirs) {
        bool matchFound = false;
        //for (auto thisit = onedirs->begin(); thisit != onedirs->end(); thisit++) {
        for (auto &oneElement : *onedirs) {
            if (twoElement->path().lexically_relative(baseTwo).compare(
                    oneElement->path().lexically_relative(baseOne)
            ) == 0) {
                oneElement->setFound();
                matchFound = true;
                differences->emplace(oneElement, SeaDoggo::DiffStates::equal);
                _recursiveDifference(oneElement, twoElement, differences, baseOne, baseTwo);
            }
        }
        if (!matchFound) {
            differences->emplace(twoElement, SeaDoggo::DiffStates::dirMissingThis);
            _recursiveDifference(nullptr, twoElement, differences, baseOne, baseTwo);
        }
    }

    //for (auto thisit = onedirs->begin(); thisit != onedirs->end(); thisit++) {
    for (auto &oneElement : *onedirs) {
        if (!oneElement->isFound()) {
            differences->emplace(oneElement, SeaDoggo::DiffStates::dirMissingOther);
            _recursiveDifference(oneElement, nullptr, differences, baseOne, baseTwo);
        }
        oneElement->resetFound();
    }
}

std::wostream &operator<<(std::wostream &os, const FSModel &model) {
    os << L"root: " << model.getRoot()->path()
       << L"\nChild Directories: " << model.getRoot()->recDirCount()
       << L"\nChild Files: " << model.getRoot()->recFileCount() << std::endl;
    return os;
}

std::wostream &operator<<(std::wostream &os, const FSEntityStateMap &map) {
    for (auto &p:map) {
        os << L"{" << p.first << L" : " << SeaDoggo::diffstate_strings.at(p.second) << L"}\n";
    }
    return os;
}

std::wostream &operator<<(std::wostream &os, const DiffStateMap &map) {
    for (auto &p:map) {
        os << L"-" << SeaDoggo::diffstate_strings.at(p.first) << L" : \n";
        os << L"\t\t" << p.second.size();
        os << L"\n";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const DiffStateMap &map) {
    for (auto &p:map) {
        os << "-" << toString(SeaDoggo::diffstate_strings.at(p.first)) << " : \n";
        os << "\t\t" << p.second.size();
        os << "\n";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const FSEntityStateMap &map) {
    for (auto &p:map) {
        os << "{" << p.first << " : " << toString(SeaDoggo::diffstate_strings.at(p.second)) << "\n";
    }
    return os;
}