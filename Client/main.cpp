#include <cassert>
#include <iostream>
#include "fsmodel.h"
#include "commandreceiver.h"
#include "jsonparser.h"
#include <string>

#define TEST

typedef bool (*Testfunction)();

typedef std::vector<Testfunction> Testfunctions;

void test(const Testfunctions &);

bool test_fsmodel();

bool test_commandreceiver();

bool test_jsonparser();

bool test_timeconversion();

int main(int argc, char **argv) {
#ifdef TEST
    test(
            {
                    test_fsmodel,
                    test_timeconversion,
                    test_jsonparser,
                    test_commandreceiver
            }
    );
#endif
    return 0;
}

bool test_fsmodel() {
    std::wcout << "~test_fsmodel~\n";
    FSModel model1;
    FSModel model2;
    FSModel model3;

    model1.discoverFSStructure(R"(D:\Alltheotherthings\Archiv\Video\)");
    model2.discoverFSStructure(R"(D:\Alltheotherthings\Archiv\Bilder\)");

    //std::wcout << model1;
    //std::wcout << model2;
    FSEntityStateMap esm = model1.performDiff(model2);
    DiffStateMap dsm = FSModel::translateMap(esm);
    //std::wcout << dsm;
    return true;
}

bool test_jsonparser() {
    std::wcout << "~test_jsonparser~\n";
    FSModel model1;
    FSModel model2;

    model1.discoverFSStructure(R"(D:\Alltheotherthings\Archiv\Video\)");
    model2.discoverFSStructure(R"(D:\Alltheotherthings\Archiv\Bilder\)");

    FSEntityStateMap esm = model1.performDiff(model2);
    DiffStateMap dsm = FSModel::translateMap(esm);

    std::stringstream sstream;
    sstream << dsm;
    std::string origin = sstream.str();
    sstream.str(std::string());

    // diffstate map saving and loading
    std::ofstream output("blob.json");
    output << JSONParser::parseToJSON(dsm);
    std::ifstream input("blob.json");
    std::string json_in((std::istreambuf_iterator<char>(input)),
                        (std::istreambuf_iterator<char>()));

    DiffStateMap refurbished_inputmap = JSONParser::readInDSM(toWString(json_in));
    sstream << refurbished_inputmap;
    //std::cout << origin;
    //std::cout << "......................\n";
    //std::cout << sstream.str() << std::endl;
    return sstream.str() == origin;
}

bool test_timeconversion() {
    std::wcout << "~test_timeconversion~\n";
    std::string blob = toISOTime(std::chrono::system_clock::now());
    if (blob.size() != sizeof("2021-04-08T12:14:57Z")) {
        //std::cout << "timeformat: " << blob;
        return false;
    }
    return true;
}

bool test_commandreceiver() {
    CommandReceiver cmr("localhost", "test:pass");
    std::vector<std::string> to_fetch = cmr.listNSend(fs::path("D:/") / "test");
    for (auto &str:to_fetch) {
        cmr.sendFile(str);
    }
    return true;
}

void test(const Testfunctions &tf) {
    for (Testfunction t: tf) {
        if (t())
            std::wcout << "success\n";
        else
            std::wcout << "failed\n";
    }
}