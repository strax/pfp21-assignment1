#include <iostream>
#include <iterator>
#include <filesystem>
#include <vector>

#include "io/memory_mapped_file.h"
#include "io/file_writer.h"
#include "vbyte.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << getprogname() << " file" << EOL;
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
        io::file_writer outfile(outpath);

        uint64_t previous = 0;
        auto output = outfile.begin();
        for (const auto &n : infile) {
            uint64_t delta = n - previous;
            previous = n;
            vbyte::encode(&output, delta);
        }
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
