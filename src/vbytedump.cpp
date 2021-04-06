#include <iostream>
#include <iterator>
#include <filesystem>
#include <cstdio>

#include "io/memory_mapped_file.h"
#include "vbyte.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file_writer\n", getprogname());
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        io::memory_mapped_file input(inpath);

        auto it = input.begin();
        while (it != input.end()) {
            uint64_t decoded = vbyte::decode(&it);
            std::cout << decoded << std::endl;
        }
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
