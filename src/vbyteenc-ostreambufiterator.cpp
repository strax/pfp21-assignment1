#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <bit>

#include "io/memory_mapped_file.h"
#include "io/file_writer.h"
#include "vbyte.h"
#include "utils.h"

static_assert(std::endian::native == std::endian::little, "Big-endian systems are not currently supported");

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);

    if (argc < 2) {
        std::cerr << "usage: " << getprogname() << " file_writer\n";
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".vb");

        io::memory_mapped_file<uint64_t> infile(inpath);
        std::ofstream ostream(outpath, std::ios::out | std::ios::binary);
        auto output = std::ostreambuf_iterator(ostream);
        for (const auto &n : infile) {
            vbyte::encode(&output, n);
        }
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
