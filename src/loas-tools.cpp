#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>
#include <regex>

#include "loas-tools.h"

using namespace std::filesystem;


void usage() {
    std::cout << "Usage: loas-tools [options] <input> \n";
    std::cout << "Options:\n";
    std::cout << "-D\tDecode mode(FakeWave to latm)\n";
    std::cout << "-F\tFake mode (latm to FakeWave)\n";
    return;
}
void cli_parser(char* argopts[], int optsum) {
    int i;
    //std::cout << optsum << "\n";
    std::ifstream ifs(argopts[optsum - 1]);
    if (!ifs.is_open()) {
        std::cout << "[Error]Failed read LATM/LAOS or FakeWaveLATM/LOAS File\n\n";
        return;
    }

    for (i = 0; i < optsum; i++) {
        //std::cout << argopts[i] << "\n";
        if (!_stricmp(argopts[i], "-D")) {
            FakeWave2Loas(argopts[optsum-1]);
        }
        else if (!_stricmp(argopts[i], "-F")) {
            Loas2FakeWave(argopts[optsum-1]);
        }
        //source = argopts[optsum];
    }
    return;
}

int main(int argc, char* argv[])
{
    std::cout << "loas-tools Version " << Version << "\n\n";


    if (!argv[2]) {
        std::cout << "[Error]No Input LATM/LAOS or FakeWaveLATM/LOAS File\n\n";
        usage();
        return 0;
    }
    if (!argv[1]) {
        usage();

        return 0;
    }
    cli_parser(argv, argc);

}
