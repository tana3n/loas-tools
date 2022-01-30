//FakeWave2Loas.cpp

#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>
using namespace std;

#define Version "1.0.0"

void FakeWave2Loas(const char* source) {
    uintmax_t size = filesystem::file_size(source);
    std::cout << "[Info]Filesize: " << double(size)/1024/1024<< " Mbytes\n";

    std::ifstream import_fakelatm(source, std::ios::in | std::ios::binary);
    if (!import_fakelatm) {
        std::cout << "[Error]Can not open source: " << source;
        return;
    }
    //std::cout << "Searching Header\n";
    char* hBuf = new char[6];

    unsigned char* header = (unsigned char*)hBuf;
    std::ofstream output_wav;
    filesystem::path p = source;
    filesystem::path filename = p.replace_extension(".latm");

    std::cout << "Set OutputFile: " << filename << std::endl;

    output_wav.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    int i = 40;//Skipping WAVE header

    while (import_fakelatm.tellg() < size) {
        import_fakelatm.seekg(i);
        import_fakelatm.read(hBuf, 6);
        if ((hBuf[0] != 0x56)) {
            //std::cout << "hBuf[" << i << "] is not 0x56. This byte is 0x" << std::hex << (unsigned int)(unsigned char)hBuf[0] << std::endl;
            i = 1 +(int)i;
            //std::cout << "Next Byte is " << i  << std::endl;

            continue;
        }
        if ((hBuf[1] & 0xE0) != 0xe0) {
            i = 1 + i;
            continue;
        }
        int length = ((((((unsigned char*)hBuf)[1] & 0x1F) << 8) | ((unsigned char*)hBuf)[2]) + 3);
        char* fBuf = new char[length];
        import_fakelatm.seekg(i);
        import_fakelatm.read(fBuf, length);
        output_wav.write(fBuf, length);
        std::cout << "\r[" << std::setfill('0') << std::left <<std::setw(4) <<  std::floor(double(i+length) / (double)size*10000)/100 
            << "%]";//Output " << double((size_t)i + length)/1024/1024 << "Mbytes" ;

        i = i + length;
       // if (i >= 10000) { break; }
    }

}
