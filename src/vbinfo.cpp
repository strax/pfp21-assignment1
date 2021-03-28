#include <iostream>
#include <ranges>
#include <filesystem>
#include <sysexits.h>
#include <vector>
#include <algorithm>
#include "io/memory_mapped_file.h"
#include <chrono>

#include "vbyte.h"

constexpr auto PROGRAM_NAME = "vbinfo";

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
        exit(EX_USAGE);
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

        std::cout << "Compressed size: " << compressed_size << " bytes" << std::endl;
        std::cout << "Raw size: " << raw_size << " bytes" << std::endl;
        std::cout << "Compression ratio: " << compression_ratio << ":1" << std::endl;
        std::cout << "Datum count: " << data.size() << std::endl;
        std::cout << "Minimum: " << *std::ranges::min_element(data) << std::endl;
        std::cout << "Maximum: " << *std::ranges::max_element(data) << std::endl;
        std::cout << "Parse time: " << parse_time.count() << "ms" << std::endl;

        return EX_OK;
    } catch (std::exception &err) {
        std::cerr << PROGRAM_NAME << ": " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
