#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <bit>
#include <array>

#include "io/memory_mapped_file.h"
#include "io/file_writer.h"
#include "vbyte.h"
#include "utils.h"

static_assert(std::endian::native == std::endian::little, "Big-endian systems are not currently supported");

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file_writer\n", getprogname());
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".vb");

        io::memory_mapped_file<uint64_t> infile(inpath);
        // Tell the kernel that pages are accessed sequentially, this might or might not help performance
        infile.advise(io::advise::sequential);
        io::file_writer outfile(outpath);
        auto output = outfile.begin();
        for (const auto &n : infile) {
            vbyte::encode(&output, n);
        }
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
