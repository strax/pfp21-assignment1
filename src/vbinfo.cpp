#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <chrono>

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
        io::memory_mapped_file infile(inpath);

        auto parse_begin = std::chrono::steady_clock::now();
        std::vector<uint64_t> data;
        auto it = infile.begin();
        while (it != infile.end()) {
            uint64_t decoded = vbyte::decode(&it);
            data.emplace_back(decoded);
        }
        auto parse_end = std::chrono::steady_clock::now();
        auto parse_time = std::chrono::duration_cast<std::chrono::milliseconds>(parse_end - parse_begin);
        std::ranges::sort(data);
        auto raw_size = data.size() * sizeof(uint64_t);
        auto compressed_size = infile.size();
        auto compression_ratio = static_cast<double>(raw_size) / compressed_size;

        std::cout << "Compressed size: " << compressed_size << " bytes" << EOL;
        std::cout << "Raw size: " << raw_size << " bytes" << EOL;
        std::cout << "Compression ratio: " << compression_ratio << ":1" << EOL;
        std::cout << "Datum count: " << data.size() << EOL;
        std::cout << "Minimum: " << *std::ranges::min_element(data) << EOL;
        std::cout << "Maximum: " << *std::ranges::max_element(data) << EOL;
        std::cout << "Parse time: " << parse_time.count() << "ms" << EOL;

        return EXIT_SUCCESS;
    } catch (std::exception &err) {
        error(err);
        exit(EXIT_FAILURE);
    }
}
