#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <windows.h>

//IMAGE_FILE_MACHINE_X Flags
//https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#machine-types for the entire list
std::unordered_map<unsigned short, std::string> machineTypeMap = {
    {IMAGE_FILE_MACHINE_UNKNOWN, "Unknown machine"},
    {IMAGE_FILE_MACHINE_I386, "x86 (32-bit)"}, //Most popular
    {IMAGE_FILE_MACHINE_AMD64, "x64 (64-bit)"}, //Most popular
    {IMAGE_FILE_MACHINE_R3000, "MIPS little-endian"},
    {IMAGE_FILE_MACHINE_R4000, "MIPS little-endian"},
    {IMAGE_FILE_MACHINE_R10000, "MIPS little-endian"},
    {IMAGE_FILE_MACHINE_WCEMIPSV2, "MIPS little-endian WCE v2"},
    {IMAGE_FILE_MACHINE_ALPHA, "Alpha_AXP"},
    {IMAGE_FILE_MACHINE_SH3, "SH3 little-endian"},
    {IMAGE_FILE_MACHINE_SH3DSP, "SH3DSP"},
    {IMAGE_FILE_MACHINE_SH3E, "SH3E little-endian"},
    {IMAGE_FILE_MACHINE_SH4, "SH4 little-endian"},
    {IMAGE_FILE_MACHINE_SH5, "SH5"},
    {IMAGE_FILE_MACHINE_ARM, "ARM Little-Endian"},
    {IMAGE_FILE_MACHINE_THUMB, "ARM Thumb/Thumb-2 Little-Endian"},
    {IMAGE_FILE_MACHINE_AM33, "Matsushita AM33"},
    {IMAGE_FILE_MACHINE_POWERPC, "IBM PowerPC Little-Endian"},
    {IMAGE_FILE_MACHINE_POWERPCFP, "IBM PowerPC Little-Endian with floating point support"},
    {IMAGE_FILE_MACHINE_IA64, "Intel Itanium processor family"},
    {IMAGE_FILE_MACHINE_MIPS16, "MIPS16"},
    {IMAGE_FILE_MACHINE_MIPSFPU, "MIPS with FPU"},
    {IMAGE_FILE_MACHINE_MIPSFPU16, "MIPS16 with FPU"},
    {IMAGE_FILE_MACHINE_ALPHA64, "Alpha_AXP_64"},
    {IMAGE_FILE_MACHINE_ARM64, "ARM64 Little-Endian"},
    {IMAGE_FILE_MACHINE_CEE, "clr pure MSIL"} 
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size];
    if (!file.read(buffer, size))
    {
        std::cerr << "Failed to read file: " << argv[1] << std::endl;
        delete[] buffer;
        return 1;
    }

    file.close();

    //Validation of file format(need to be a binary file)
    if (size < sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS))
    {
        std::cerr << "Invalid file format: " << argv[1] << std::endl;
        delete[] buffer;
        return 1;
    }

    //Validation of the DOS Header
    IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(buffer);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        std::cerr << "Invalid DOS header: " << argv[1] << std::endl;
        delete[] buffer;
        return 1;
    }

    //Validation of NT Headers that are used to know the file compilation type
    IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(buffer + dosHeader->e_lfanew);
    if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        std::cerr << "Invalid NT header: " << argv[1] << std::endl;
        delete[] buffer;
        return 1;
    }

    //Extract the type using flags
    std::string machineType = machineTypeMap[ntHeader->FileHeader.Machine];

    std::string path = argv[1];

    size_t last_backslash = path.find_last_of("\\");

    //Example output: "[File.exe] x64 (64-bit)" if it does contain any Path it will get cutted to only say the file name.
    std::cout << "[" << path.substr(last_backslash + 1) << "] " << machineType << std::endl;

    delete[] buffer;

    system("pause");

    return 0;
}