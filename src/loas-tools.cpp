#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>
#include <regex>

#include "loas-tools.h"

using namespace std::filesystem;

int main(int argc, char* argv[])
{
    std::cout << "loas-tools Version " << Version << "\n\n";


    if (!argv[1]) {
        std::cout << "[Error]No Input LATM/LAOS or FakeWaveLATM/LOAS File\n\n";
        usage();

        return 0;
    }
    cli_parser(argv, argc);

}


void usage() {
    std::cout << "Usage: loas-tools [options] <input> \n";
    std::cout << "Options:\n";
    std::cout << "-D\tDecode mode(FakeWave to latm)\n";
    std::cout << "-F\tFake mode (latm to FakeWave)\n\n";
    std::cout << "--bit\tSelecting WaveHeader blocksize (for Fake mode)\n\t8:\t8bit\n\t16:\t16bit\n\t24:\t24bit(default)\n";

    std::cout << "--overwrite\tIf it exists in the output destination, it will be overwritten\n";
    std::cout << "--exact\tOutputs the deviation correction information of the start point(This is vaild for matroska)\n";

    return;
}
void cli_parser(char* argopts[], int optsum) {
    //std::cout << optsum << "\n";
    std::ifstream ifs(argopts[optsum - 1]);
    if (!ifs.is_open()) {
        std::cout << "[Error]Failed read LATM/LAOS or FakeWaveLATM/LOAS File\n\n";
        return;
    }
    struct _opts option;

    option.overwrite = false;
    option.exact = false;
    option.bitdepth = 24;
    bool set_d = false;
    bool set_f = false;
    path src = argopts[optsum - 1];

    if (src.extension() ==".latm"){
        set_f = true;
        std::cout << "[Info]Auto Set: Fake mode\n";

    } else if (src.extension() == ".wav") {
        set_d = true;
        std::cout << "[Info]Auto Set: Decode mode\n";

    }

    for (int i = 0; i < optsum; i++) {
        if (!_stricmp(argopts[i], "-D")) {
            set_d = true;
            std::cout << "[Info]Set Decode mode\n";
        }
        else if (!_stricmp(argopts[i], "-F")) {
            set_f = true;
            std::cout << "[Info]Set Fake mode\n";
        }
        if (!_stricmp(argopts[i], "--overwrite")) {
            option.overwrite = true;
            continue;
        }
        if (!_stricmp(argopts[i], "--exact")) {
            option.exact = true;
            continue;
        }
        if (!_stricmp(argopts[i], "--bit")) {
            option.bitdepth = argopts[i+1][0];
            continue;
        }
    }

    if (!(set_f ^ set_d)) {
        std::cout << "[Error]Invaild mode option" << std::endl;
        usage();
        return;
        
    }else if (set_d) {
        FakeWave2Loas(argopts[optsum - 1], &option);
    }
    else if (set_f) {
        Loas2FakeWave(argopts[optsum - 1], &option);
    }
    return;
}

