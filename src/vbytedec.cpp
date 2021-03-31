#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <functional>

#include "io/memory_mapped_file.h"
#include "vbyte.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file\n", getprogname());
        exit(EXIT_FAILURE);
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
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
