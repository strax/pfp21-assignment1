#include <iostream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>

#include "io/memory_mapped_file.h"
#include "vbyte.h"

constexpr auto PROGRAM_NAME = "vbytedump";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        exit(EX_USAGE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        io::memory_mapped_file input(inpath);

        auto it = input.begin();
        while (it != input.end()) {
            uint64_t decoded = vbyte::decode(&it);
            std::cout << decoded << std::endl;
        }
        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
