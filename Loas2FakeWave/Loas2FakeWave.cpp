//Loas2FakeWave.cpp
#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>
using namespace std::filesystem;

constexpr int Header = 56;
#define Version "1.0.0"

void Loas2FakeWave(const char* source);
void usage();

int main(int argc, char* argv[])
{
    std::cout << "Loas2FakeWave Version " << Version << "\n\n";

    if (argc < 2) {
        std::cout << "[Error]No Input LATM/LAOS File\n";
        usage();

        return 0;
    }
    std::cout << "InputFile: " << argv[1] << "\n";

    Loas2FakeWave(argv[1]);
}

void Loas2FakeWave(const char* source) {
    std::uintmax_t size = file_size(source);
    std::cout << "Filesize: " << size << " bytes\n";

    std::ifstream import_latm(source, std::ios::in | std::ios::binary);
    if (!import_latm) {
        std::cout << "Can not open source: " << source;
        return;
    }
    std::cout << "Searching Header\n";
    char* hBuf = new char[6];

    int i = 0;
    unsigned char* header = (unsigned char*)hBuf;
    for (i = 0; i < 1000; i++) {//getSyncHeader  (size / 10)
        if (i + 7 >= size) {
            break;
        }

        import_latm.seekg(i);
        import_latm.read(hBuf, 6);
        if (hBuf[0] != 0x56) {
            //std::cout << "[LatmDecorder]hBuf["<< i<<"] is not 0x56. This byte is 0x" << std::hex << (unsigned int)(unsigned char)hBuf[0] << std::endl;
            continue;
        }

        std::cout << "import_latm[" << i << "] is 0x56. Checking next byte" << std::endl;

        if ((hBuf[1] & 0xE0) != 0xe0) {
            std::cout << "import_latm[" << i + 1 << "]is not 0xE0. This byte is 0x" << std::hex << (unsigned int)(unsigned char)hBuf[1] << std::endl;
            continue;
        }
        std::cout << "import_latm[" << i + 1 << "] is 0xE0. Done SyncHeader" << std::endl;
        import_latm.seekg(i);
        break;
    }
    std::ofstream output_wav;
    path p = source;
    path filename = p.replace_extension(".wav");
    std::cout << "SetOutput: " << filename << std::endl;

    output_wav.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    output_wav.write("RIFF", 4);
    output_wav.write("0000", 4);//size-8bytes
    output_wav.write("WAVE", 4);
    output_wav.write("fmt ", 4);
    int s = 16; //16Bit
    output_wav.write((char*)&s, 4);
    int t = 1; //PCM
    output_wav.write((char*)&t, 2);
    int t1 = 1; //ch
    output_wav.write((char*)&t1, 2);
    int t2 = 48000; //KHz
    output_wav.write((char*)&t2, 4);
     int t3 = 0x00017700;
    output_wav.write((char*)&t3, 4);
    int t4 = 0x0002; 
    output_wav.write((char*)&t4, 2); //l
    output_wav.write((char*)&s, 2);
    output_wav.write("data", 4);
    output_wav.write("0000", 4);//size-126b 
    char out_fill[4096] = { 0 };
    std::cout << "\n";

    while (import_latm.tellg() < size) {
        int i = import_latm.tellg();
        import_latm.read(hBuf, 6);

        int length = ((((((unsigned char*)hBuf)[1] & 0x1F) << 8) | ((unsigned char*)hBuf)[2]) + 3);
        //std::cout << "This Sample length is " << length << " bytes" << std::endl;

        char* fBuf = new char[length];

        import_latm.seekg(i);
        import_latm.read(fBuf, length);

        int sss = output_wav.tellp();
        output_wav.write(out_fill, 2048);
        output_wav.seekp(sss);
        output_wav.write(fBuf, length);
        //output_wav.write("END", 3);
        output_wav.seekp((size_t)sss + 2048);

        std::cout << "\rOutput " << i + length << "bytes";
        import_latm.seekg((size_t)i + length);
    }
    int size2 = file_size(filename);
    output_wav.seekp(4);
    int headers2 = size2 - 4;
    output_wav.write((char*)&headers2, 4);
    output_wav.seekp(40);
    int headers3 = size2 - 126;
    output_wav.write((char*)&headers3, 4);
    output_wav.close();
    //unsigned char* header{} = header + 1;//next bytes

}

void usage() {
    std::cout << "Usage: Loas2FakeWave input.latm \n";
    return;
}