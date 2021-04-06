#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <vector>

#include "io/memory_mapped_file.h"
#include "io/file.h"
#include "vbyte.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file\n", getprogname());
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        io::memory_mapped_file<uint64_t> infile(inpath);
        std::filesystem::path outpath(inpath);

        std::vector<uint64_t> inputs;
        std::copy(infile.begin(), infile.end(), std::back_inserter(inputs));
        std::sort(inputs.begin(), inputs.end());

        outpath.replace_extension(outpath.extension().string() + ".sorted.vb");
        io::file outfile(outpath, io::mode::write);

        uint64_t previous = 0;
        std::vector<std::byte> buffer;
        buffer.reserve(io::page_size * 3);
        auto output = std::back_inserter(buffer);
        for (const auto &n : infile) {
            uint64_t delta = n - previous;
            previous = n;
            vbyte::encode(&output, delta);
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
