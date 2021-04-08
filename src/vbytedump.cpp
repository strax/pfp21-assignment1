#include <iostream>
#include <iterator>
#include <filesystem>

#include "io/memory_mapped_file.h"
#include "vbyte.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << getprogname() << " file" << EOL;
        exit(EXIT_FAILURE);
    }

    try {
        std::filesystem::path inpath(argv[1]);
        io::memory_mapped_file input(inpath);

        auto it = input.begin();
        while (it != input.end()) {
            uint64_t decoded = vbyte::decode(&it);
            std::cout << decoded << EOL;
        }
        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
