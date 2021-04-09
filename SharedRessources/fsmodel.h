//
// Created by Chrystalkey on 12.11.2020.
//

#ifndef SEADOGGO_FSMODEL_H
#define SEADOGGO_FSMODEL_H
// a class that models the tree-structure of the file tree that is watched.
// TODO: perform diffs/update FSEntity by path/

#include <map>
#include <list>
#include <ostream>
#include <unordered_set>

#include "fsentity.h"
#include "SDDefinitions.h"

namespace SeaDoggo{
    /**
     * Listing of the states in which a file in set A can be in relation to set B
     */
typedef enum class _DiffStates {
    equal,
    fileMissingThis,
    fileMissingOther,
    dirMissingThis,
    dirMissingOther,
    fileMtimeMoreRecentThis,
    fileMtimeMoreRecentOther,
    fileHashDifferent,
    DiffStateError
} DiffStates;

/**
 * hardcoded map with which to convert the DiffStates into corresponding string values
 */
static const
std::map<DiffStates, std::wstring> diffstate_strings = {{DiffStates::equal,                    L"equal"},
                                                        {DiffStates::fileMissingThis,          L"fileMissingThis"},
                                                        {DiffStates::fileMissingOther,         L"fileMissingOther"},
                                                        {DiffStates::dirMissingThis,           L"dirMissingThis"},
                                                        {DiffStates::dirMissingOther,          L"dirMissingOther"},
                                                        {DiffStates::fileMtimeMoreRecentThis,  L"fileMtimeMoreRecentThis"},
                                                        {DiffStates::fileMtimeMoreRecentOther, L"fileMtimeMoreRecentOther"},
                                                        {DiffStates::fileHashDifferent,        L"fileHashDifferent"},
                                                        {DiffStates::DiffStateError,                    L"Fehler"}};
/**
 * hardcoded map with which to convert the string values of DiffStates into corresponding DiffStates
 */
static const
std::map<std::wstring, DiffStates> string_diffstates = {{L"equal",                      DiffStates::equal},
                                                        {L"fileMissingThis",            DiffStates::fileMissingThis},
                                                        {L"fileMissingOther",           DiffStates::fileMissingOther},
                                                        {L"dirMissingThis",             DiffStates::dirMissingThis},
                                                        {L"dirMissingOther",            DiffStates::dirMissingOther},
                                                        {L"fileMtimeMoreRecentThis",    DiffStates::fileMtimeMoreRecentThis},
                                                        {L"fileMtimeMoreRecentOther",   DiffStates::fileMtimeMoreRecentOther},
                                                        {L"fileHashDifferent",          DiffStates::fileHashDifferent},
                                                        {L"Fehler",                     DiffStates::DiffStateError}};
/**
 * hardcoded map with which to convert DiffState Value alpha into its opposite or some equivalent notion of it
 * e.g. "equal" will be converted to "equal"
 */
static const
std::map<DiffStates, DiffStates> opposite_diffstates = {
        {DiffStates::equal,                    DiffStates::equal},
        {DiffStates::fileMissingThis,          DiffStates::fileMissingOther},
        {DiffStates::fileMissingOther,         DiffStates::fileMissingThis},
        {DiffStates::dirMissingThis,           DiffStates::dirMissingOther},
        {DiffStates::dirMissingOther,          DiffStates::dirMissingThis},
        {DiffStates::fileMtimeMoreRecentThis,  DiffStates::fileMtimeMoreRecentOther},
        {DiffStates::fileMtimeMoreRecentOther, DiffStates::fileMtimeMoreRecentThis},
        {DiffStates::fileHashDifferent,        DiffStates::fileHashDifferent},
        {DiffStates::DiffStateError,                    DiffStates::DiffStateError}
};
}
/**
 * typedef std::map<const FSEntity *, SeaDoggo::DiffStates> FSEntityStateMap;
 */
typedef std::map< sptr<const FSEntity>, SeaDoggo::DiffStates> FSEntityStateMap;
/**
 * typedef std::map<SeaDoggo::DiffStates, std::list<FSEntity*> > DiffStateMap;
 */
typedef std::map<SeaDoggo::DiffStates, std::vector< sptr<const FSEntity>> > DiffStateMap;
/**
 * typedef std::vector<const FSEntity *> FSEntityVector;
 */
typedef std::vector< sptr< const FSEntity>> FSEntityVector;
/**
 * typedef FSDir FSTree;
 */
typedef sptr<FSDir> FSTree;

/**
 * typedef std::unordered_map<std::string, std::time_t> Filelisting;
 */
typedef std::unordered_map<std::string, std::time_t> Filelisting;

/**
 * Model a Set of files for comparison with a corresponding set of files.
 */
class FSModel {
public:
    explicit FSModel() {}
    explicit FSModel(const fs::path &root){discoverFSStructure(root);}

    /**
     * Pass an argument into this function to fill the model with data. reapply when needed.
     * @param root the root directories absolute path
     */
    void discoverFSStructure(const fs::path &root);

    void createFSStructure(const std::string &source);

    /**
     * pass on another FSModel to compare it to, get the result in a FSEntityStateMap in return.
     * this model will be number 1[this, here], @other will be number 2[other, there]
     *
     * @warning FSEntityStateMap consists of FSEntities of both models. no dependencies to parent entities are
     *           necessary valid.
     * @warning only valid data while both FSModels are in existance
     * @param other the other model.
     * @return
     */
    FSEntityStateMap performDiff(FSModel &other);

    /**
     * return the root directoriy as a FSEntity
     * @return
     */
    const FSTree &getRoot() const { return root; }

    /**
     * translate the FSEntityStateMap into the opposite form for the "other" model.
     * in effect returns the FSEntityStateMap as if performDiff would have been called on the other model with this
     * as an argument
     * @param original the map to be translated
     * @return the FSEntityStateMap from the opposite model's view
     */
    static FSEntityStateMap translateToOpposite(const FSEntityStateMap &original){
        FSEntityStateMap translated;
        std::for_each(original.begin(),original.end(),[&](std::pair< sptr<const FSEntity>, SeaDoggo::DiffStates> pair){
            translated.emplace(pair.first, SeaDoggo::opposite_diffstates.at(pair.second));
        });
        return translated;
    }

    /**
     * Overloaded function to translate a FSEntityStateMap into a DiffStateMap
     * @param fsm map to be translated
     * @return DiffStateMap containing the same state of difference as the argument
     */
    static DiffStateMap translateMap(const FSEntityStateMap &fsm){
        DiffStateMap translated = {
                {SeaDoggo::DiffStates::equal,{}},
                {SeaDoggo::DiffStates::fileMissingThis,{}},
                {SeaDoggo::DiffStates::fileMissingOther,{}},
                {SeaDoggo::DiffStates::dirMissingThis,{}},
                {SeaDoggo::DiffStates::dirMissingOther,{}},
                {SeaDoggo::DiffStates::fileMtimeMoreRecentThis,{}},
                {SeaDoggo::DiffStates::fileMtimeMoreRecentOther,{}},
                {SeaDoggo::DiffStates::fileHashDifferent,{}},
                {SeaDoggo::DiffStates::DiffStateError,{}}
        };

        for(auto &pair: fsm){
            translated[pair.second].emplace_back((pair.first));
        }
        return translated;
    }

    /**
     * Overloaded function to translate a DiffStateMap into a FSEntityStateMap
     * @param fsm map to be translated
     * @return FSEntityStateMap containing the same state of difference as the argument
     */
    static FSEntityStateMap translateMap(const DiffStateMap& mappe){
        FSEntityStateMap esm;
        for(auto &pair : mappe){
            for(auto &entity : pair.second){
                esm.emplace(entity, pair.first);
            }
        }
        return esm;
    }

    /**
     * For fun an overloaded function to just output a model's stats onto streams
     * @param os stream reference in
     * @param model this model
     * @return stream reference out
     */
    friend std::wostream &operator<<(std::wostream &os, const FSModel &model);

private:
    /**
     * internal helper to discover the structure, internally working with FSEntities
     * @param parent the parent Directory Entity
     */
    void discoverFSStructure(const sptr<FSDir> &parent);

    /**
     * insert the path specified by "segments" into the tree starting at root. recursively of course.
     *
     * @param segments the path to be inserted
     * @param parent the parent directory of the subpath
     * @param mtime the mtime of the file. nullptr if it is a directory
     * @param index the index in the "segments" vector we are at
     */
    void insert_into_tree(const std::vector<std::string> &segments, sptr<FSDir> parent, const std::time_t *mtime, std::size_t index = 0);

    /**
     * internal method containing the algorithm for finding the difference between two file trees
     * @param one recursive Root directory from this side
     * @param two recursive Root directory from the other side
     * @param differences the map in which the differences are stored
     * @param baseOne the absolute root directory for this Model
     * @param baseTwo the absolute root directory for the other Model.
     */
    static void _recursiveDifference(sptr<FSDir> one, sptr<FSDir> two, FSEntityStateMap *differences,
                                     const fs::path &baseOne, const fs::path &baseTwo);

    FSTree root = nullptr;
};

std::wostream &operator<<(std::wostream &os, const FSEntityStateMap &map);
std::wostream &operator<<(std::wostream &os, const DiffStateMap &map);

std::ostream &operator<<(std::ostream &os, const DiffStateMap &map);
std::ostream &operator<<(std::ostream &os, const FSEntityStateMap &map);
#endif //SEADOGGO_FSMODEL_H
