#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <bit>

#include "vbyte.h"
#include "utils.h"

static_assert(std::endian::native == std::endian::little, "Big-endian systems are not supported");

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << getprogname() << " file" << EOL;
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".vb");

        std::ifstream istream(inpath, std::ios::in | std::ios::binary);
        std::ofstream ostream(outpath, std::ios::out | std::ios::binary);
        auto output = std::ostreambuf_iterator(ostream);
        while (!istream.eof() && !istream.bad()) {
            uint64_t n;
            istream.read(reinterpret_cast<char *>(&n), sizeof(uint64_t));
            vbyte::encode(&output, n);
        }
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
