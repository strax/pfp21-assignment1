#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>
#include <functional>

#include "io/memory_mapped_file.h"
#include "vbyte.h"

constexpr auto PROGRAM_NAME = "vbytedec";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        exit(EX_USAGE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        io::memory_mapped_file infile(inpath);

        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".dec");
        std::ofstream ostream(outpath, std::ios::out | std::ios::binary);
        auto it = infile.begin();
        while (it != infile.end()) {
            uint64_t decoded = vbyte::decode(&it);
            ostream.write((const char*) &decoded, sizeof(decoded));
        }
        ostream.flush();
        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
