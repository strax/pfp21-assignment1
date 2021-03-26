#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>

#include "vbyte.h"

constexpr auto PROGRAM_NAME = "vbytedump";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        exit(EX_USAGE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        std::ifstream istream(inpath, std::ios::in | std::ios::binary);
        auto input = std::istreambuf_iterator(istream);

        while (input != std::istreambuf_iterator<char>()) {
            uint64_t decoded = vbyte::decode(&input);
            std::cout << decoded << std::endl;
        }
        istream.close();
        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
