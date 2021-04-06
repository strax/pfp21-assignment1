#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <bit>
#include <array>

#include "io/memory_mapped_file.h"
#include "io/file.h"
#include "vbyte.h"
#include "utils.h"

static_assert(std::endian::native == std::endian::little, "Big-endian systems are not currently supported");

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);

    if (argc < 2) {
        std::cerr << "usage: " << getprogname() << " file\n";
        std::cerr << "page size: " << system_page_size() << "\n";
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".vb");

        io::memory_mapped_file<uint64_t> infile(inpath);
        infile.advise(io::advise::sequential);
        io::file outfile(outpath, io::mode::write);
        std::vector<std::byte> buffer;
        buffer.reserve(io::page_size * 3);
        auto output = std::back_inserter(buffer);
        for (const auto &n : infile) {
            vbyte::encode(&output, n);
            if (buffer.size() >= (io::page_size * 2)) {
                outfile.write(std::span{buffer.data(), buffer.size()});
                buffer.clear();
            }
        }
        outfile.write(buffer);
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
