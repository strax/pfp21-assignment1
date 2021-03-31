#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>
#include <vector>
#include <bit>

#include "io/memory_mapped_file.h"
#include "vbyte.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file\n", getprogname());
        exit(EX_USAGE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        io::memory_mapped_file<uint64_t> infile(inpath);
        std::filesystem::path outpath(inpath);

        std::vector<uint64_t> inputs;
        std::copy(infile.begin(), infile.end(), std::back_inserter(inputs));
        std::sort(inputs.begin(), inputs.end());

        outpath.replace_extension(outpath.extension().string() + ".sorted.vb");
        std::ofstream ostream(outpath, std::ios::out | std::ios::binary);
        auto output = std::ostreambuf_iterator(ostream);

        uint64_t previous = 0;
        for (const auto &n : inputs) {
            uint64_t delta = n - previous;
            previous = n;
            vbyte::encode(&output, delta);
        }
        ostream.flush();
        ostream.close();
        return EX_OK;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
