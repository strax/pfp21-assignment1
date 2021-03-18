#include <iostream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>

#include "MemoryMappedFile.h"
#include "VByte.h"
#include <ranges>

constexpr auto PROGRAM_NAME = "vbyteenc";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        exit(EX_USAGE);
    }

    try {
        auto path = std::filesystem::path(argv[1]);
        auto file = std::make_unique<MemoryMappedFile<uint64_t>>(path);
        static_assert(std::input_or_output_iterator<MemoryMappedFile<uint64_t>>);
        std::cout << "File contents: " << file->size() << " elements" << std::endl;
        std::cout << (*file)[0] << std::endl;
        std::cout << (*file)[1] << std::endl;
        std::cout << (*file)[2] << std::endl;
        std::cout << (*file)[3] << std::endl;
        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
