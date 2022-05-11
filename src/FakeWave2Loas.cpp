//FakeWave2Loas.cpp

#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>
using namespace std;

#include "FakeWave2Loas.h"
#include "loas-tools.h"

void FakeWave2Loas(const char* source,struct _opts *option) {
    uintmax_t size = filesystem::file_size(source);
    std::cout << "[Info]Filesize: " << double(size)/1024/1024<< " Mbytes\n";

    std::ifstream import_fakelatm(source, std::ios::in | std::ios::binary);
    if (!import_fakelatm) {
        std::cout << "[Error]Can not open source: " << source;
        return;
    }
    //std::cout << "Searching Header\n";
    char* iBuf = new char[48];
    import_fakelatm.read(iBuf, 48);
    uint16_t blockSize = iBuf[0x20];
    char* hBuf = new char[blockSize*1024];
    std::cout << "[Info]Initilized blocksize: " << blockSize <<"\n";

    //その前にヘッダー読んだ上で1フレームのサイズを計算しないといけないと思う
    int i = 46;//Skipping WAVE header

    while (import_fakelatm.tellg() < size) {
        import_fakelatm.seekg(i);
        import_fakelatm.read(hBuf, 6);
        if ((hBuf[0] != 0x56) || (hBuf[1] & 0xE0) != 0xe0) {
            i = 1 + (int)i;
            continue;
        }
        std::cout << i - 46 << "bytes skipped (" << (i- 46) / 4.0 << "samples DELAY " << ((i - 46) / 4.0 / 48) <<"ms)" << std::endl;
        break;
    }


    std::ofstream output_latm;
    filesystem::path p = source;
    if (option->exact){
        filesystem::path filename2 = p.replace_extension("");
        int ms = round((i - 46) / 4.0 / 48);
        std::string namerep = " DELAY " + to_string(ms) + "ms.latm";
        p = filename2.concat(namerep);
    }
    else {
        filesystem::path filename2 = p.replace_extension(".latm");
        p = filename2;
    }
    if (filesystem::exists(p) & !(option->overwrite) ) {
        std::cout <<"[Error]" << p << "is exist. (use overwrite options: --overwrite)" << std::endl;

        return;
    }
    std::cout << "Set OutputFile: " << p << std::endl;

    output_latm.open(p, std::ios::out | std::ios::binary | std::ios::trunc);
    for (int i = 0; i < size; i++) {
    //while (import_fakelatm.tellg() <= size) {//sync
        import_fakelatm.seekg(i);
        import_fakelatm.read(hBuf, blockSize * 1024);
        if ( (hBuf[0] != 0x56) || (hBuf[1] & 0xE0) != 0xe0 ){
            continue;
        }

        int length = ((((((unsigned char*)hBuf)[1] & 0x1F) << 8) | ((unsigned char*)hBuf)[2]) + 3);
        output_latm.write(hBuf, length);

        std::cout << "\r[" << std::setfill('0') << std::left <<std::setw(4) <<  std::floor(double(i+length) / (double)size*10000)/100 
            << "%]";//Output " << double((size_t)i + length)/1024/1024 << "Mbytes" ;
        import_fakelatm.read(hBuf, blockSize * 1024);
        if ((hBuf[0] != 0x56) || (hBuf[1] & 0xE0) != 0xe0) {
            continue;
        }
        else {
            i = i + blockSize * 1024 - 1;//== ch * (Bit/8)
        }
        
    }
    return;
}
