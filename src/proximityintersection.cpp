#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <sysexits.h>
#include <bit>
#include <vector>
#include <chrono>
#include <algorithm>

#include "io/memory_mapped_file.h"
#include "vbyte.h"

namespace fs = std::filesystem;

static_assert(std::endian::native == std::endian::little, "Big-endian systems are not currently supported");

constexpr auto PROGRAM_NAME = "proximityintersection";

auto read_index_file(fs::path& path) {
    std::ifstream file(path);
    std::vector<std::pair<uint16_t, uint16_t>> pairs;
    uint16_t a, b;
    while (file >> a >> b) {
        pairs.emplace_back(a, b);
    }
    return pairs;
}

std::vector<uint64_t> proximity_intersection(uint64_t radius, std::vector<uint64_t> as, std::vector<uint64_t> &bs) {
    std::ranges::sort(as);
    for (const auto b : bs) {
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <radius> <filename>" << std::endl;
        exit(EX_USAGE);
    }

    int radius = std::stoi(std::string(argv[1]));
    std::cerr << "radius: " << std::to_string(radius) << std::endl;
    fs::path inpath(argv[2]);

    auto pairs = read_index_file(inpath);
    auto begin = std::chrono::steady_clock::now();
    std::vector<size_t> sizes;
    sizes.reserve(pairs.size());

    for (const auto& pair : pairs) {
        // sizes.emplace_back(proximity_intersection(pair.first, pair.second));
    }
    std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - begin;
    std::cout << "Total duration: " << elapsed.count() << std::endl;
    return EX_OK;
}
