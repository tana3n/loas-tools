//Loas2FakeWave.cpp
#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>
#include <regex>

#include "Loas2FakeWave.h"

using namespace std::filesystem;


void Loas2FakeWave(const char* source) {
    std::cout << "InputFile: " << source << "\n";
    std::uintmax_t size = file_size(source);
    //std::cout << "Filesize: " << size << " bytes\n";

    std::ifstream import_latm(source, std::ios::in | std::ios::binary);
    if (!import_latm) {
        std::cout << "Can not open source: " << source;
        return;
    }
    
    std::regex re(R"((?!(.+DELAY ))[-,0-9]{1,6})" );
    std::cmatch m;
    std::regex_search(source, m, re);
    double samples = 48000.0 * (stoi(m[0].str()) / 1000.0);
    std::ofstream output_wav;
    path p = std::regex_replace(source, re, "0");
    path filename = p.replace_extension("wav");
    std::cout << "SetOutputFile: " << filename << std::endl;
    std::cout << "DELAY  " << m[0].str() << "ms" << "(" << samples << "samples)" << std::endl;
    double offsets = samples/1024.0;
    double patting = (floor(offsets) * 1024.0 - samples) * 4;
    //std::cout << "Exaxtry Offsets: " << ceil(offsets) << "Frames + " << fmod(samples, 1024) << "samples(DELAY " << fmod(samples, 1024) / 48 << "ms)" << std::endl;
    std::cout << "Exactly Offsets: " << floor(offsets) << "Frames +" << patting / 4.0 << "Sample (DELAY " << (samples - floor(offsets) * 1024.0) / 48 << "ms)" << std::endl;


    //std::cout << "Searching Header\n";
    char* hBuf = new char[6];

    int i = 0;
    unsigned char* header = (unsigned char*)hBuf;
    int fr = 0;
    for (i = 0; i < 100000; i++) {//getSyncHeader  (size / 10)
        if (i + 7 >= size) {
            break;
        }

        import_latm.seekg(i);
        import_latm.read(hBuf, 6);
        if (hBuf[0] != 0x56) {
            //std::cout << "[LatmDecorder]hBuf["<< i<<"] is not 0x56. This byte is 0x" << std::hex << (unsigned int)(unsigned char)hBuf[0] << std::endl;
            continue;
        }

        //std::cout << "0x" << i << " is 0x56. Checking next byte" << std::endl;

        if ((hBuf[1] & 0xE0) != 0xe0) {
           // std::cout << "0x" << i << " is not 0xE0. This byte is 0x" << std::hex << (unsigned int)(unsigned char)hBuf[1] << std::endl;
            continue;
        }
        //std::cout << "0x" << i << " is 0xE0. Done SyncHeader" << std::endl;
        import_latm.seekg(i);
        break;
    }
    std::cout << "Done Searching Header\n";

    while(fr < (std::abs(floor(offsets)))) {
        std::uintmax_t j = import_latm.tellg();
        import_latm.read(hBuf, 6);
        std::uintmax_t length = ((((((unsigned char*)hBuf)[1] & 0x1F) << 8) | ((unsigned char*)hBuf)[2]) + 3);
        
        //std::cout << "Skipping  " << std::dec << fr << " of " << std::abs(floor(offsets)) << std::endl;
        import_latm.seekg(j+length);
        fr += 1;
    }

    std::cout << "Done Frame Skipping" << std::endl;
    std::cout << "CurrentSector: " << import_latm.tellg() << std::endl;

    struct _waveheader wave;
    wave.riff.chunkId = *reinterpret_cast<const uint32_t*>("RIFF");
    wave.wave.chunkId = *reinterpret_cast<const uint32_t*>("WAVE");
    wave.wave.fmt = *reinterpret_cast<const uint32_t*>("fmt ");
    wave.wave.chunkSize = 16;
    wave.wave.bitdepth = 16;
    wave.wave.channel = 2;
    wave.wave.formatTag = 1;
    wave.wave.samplingRate = 48000;
    wave.wave.blockSize = wave.wave.channel * (wave.wave.bitdepth / 8);
    wave.wave.byteRate = wave.wave.blockSize * wave.wave.samplingRate; // 0x00017700;
    wave.data.chunkId = *reinterpret_cast<const uint32_t*>("data");

    char out_fill[4096] = { 0 };

    output_wav.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    output_wav.seekp(sizeof(wave));

    if (patting < 0) {
        std::cout << "Delay correcting:  " << (floor(offsets) * 1024.0 - samples) << "sample(" << patting << "bytes)" << std::endl;
        output_wav.write(out_fill, std::abs(patting));
    }

    int sp = 0;

    while (import_latm.tellg() < size) {
        sp = sp + 1;
        i = import_latm.tellg();
        import_latm.read(hBuf, 6);

        int length = ((((((unsigned char*)hBuf)[1] & 0x1F) << 8) | ((unsigned char*)hBuf)[2]) + 3);
        //std::cout << "This Sample length is " << length << " bytes" << std::endl;

        char* fBuf = new char[length];

        import_latm.seekg(i);
        import_latm.read(fBuf, length);

        std::uintmax_t sss = output_wav.tellp();
        output_wav.write(out_fill, 4096);
        output_wav.seekp(sss);
        output_wav.write(fBuf, length);
        //output_wav.write("END", 3);
        output_wav.seekp((size_t)sss + 4096);
        std::cout << 
            "[" << std::setfill('0') << std::left << std::setw(4) << std::floor(double(i + length) / (double)size * 10000) / 100 << "%]\r";
        //std::cout << "\rOutput " << i + length << "bytes " << sp << "frames";
        import_latm.seekg((size_t)i + length);
    }
    std::cout << "\rOutput " << size << "bytes (" << sp << "frames)";

    std::uintmax_t size2 = file_size(filename);

    output_wav.seekp(0);
    wave.riff.chunkSize = size2 - 4;
    wave.data.chunkSize = size2 - 126;
    output_wav.write((char*)&wave, sizeof(wave));

    output_wav.close();
}