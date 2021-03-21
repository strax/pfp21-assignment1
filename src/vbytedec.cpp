#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>
#include <functional>

#include "MemoryMappedFile.h"
#include "VByte.h"

constexpr auto PROGRAM_NAME = "vbytedec";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        exit(EX_USAGE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        std::ifstream istream(inpath, std::ios::in | std::ios::binary);
        auto input = std::istreambuf_iterator(istream);

        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".dec");
        std::ofstream ostream(outpath, std::ios::out | std::ios::binary);
        while (input != std::istreambuf_iterator<char>()) {
            uint64_t decoded = VByteDecodeInteger(input);
            ostream.write((const char*) &decoded, sizeof(decoded));
        }
        istream.close();
        ostream.close();
        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
