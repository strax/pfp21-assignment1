#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>
#include <functional>

#include "MemoryMappedFile.h"
#include "VByte.h"

constexpr auto PROGRAM_NAME = "vbyteenc";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        exit(EX_USAGE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        auto infile = std::make_unique<MemoryMappedFile<uint64_t>>(inpath);
        std::filesystem::path outpath(inpath);
        outpath.replace_extension(outpath.extension().string() + ".vb");
        std::ofstream ostream(outpath);
        auto output = std::ostreambuf_iterator(ostream);
        for (int i = 0; i < infile->size(); i++) {
            VByteEncodeInteger(output, (*infile)[i]);
        }
        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
