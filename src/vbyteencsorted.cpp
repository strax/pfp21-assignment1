#include <iostream>
#include <iterator>
#include <filesystem>
#include <vector>
#include <numeric>

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
        // Not sure if std::copy is sequential or not
        infile.advise(io::advise::willneed);
        std::filesystem::path outpath(inpath);

        // Create a separate vector with the same size as the mapped region - the constructor variant actually creates the uints
        // instead of just reserving size (so we can use `.begin()`)
        std::vector<uint64_t> inputs(infile.size());
        std::copy(infile.begin(), infile.end(), inputs.begin());
        // This could be potentially accelerated with execution policies, but GCC requires linking with Intel TBB
        // and external libraries were forbidden in the assignment brief
        std::sort(inputs.begin(), inputs.end());

        outpath.replace_extension(outpath.extension().string() + ".sorted.vb");
        io::file_writer outfile(outpath);

        uint64_t previous = 0;
        auto output = outfile.begin();
        for (const auto &n : inputs) {
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
