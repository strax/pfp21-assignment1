#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <bit>
#include <vector>
#include <chrono>
#include <algorithm>

#include "io/memory_mapped_file.h"
#include "vbyte.h"
#include "utils.h"

namespace fs = std::filesystem;

static_assert(std::endian::native == std::endian::little, "Big-endian systems are not supported");

auto read_index_file(fs::path& path) {
    if (!fs::exists(path)) {
        throw std::runtime_error(path.string() + ": no such file");
    }
    std::ifstream file(path, std::ios::in);
    std::vector<std::pair<uint16_t, uint16_t>> pairs;
    uint16_t a, b;
    while (file >> a >> b) {
        pairs.emplace_back(a, b);
    }
    return pairs;
}


/// Performs proximity intersection on the given two sets \a as and \a bs with radius \a radius.
/// At least \a as is required to be sorted, but performance seems to be better if \b is sorted too.
/// We use \a std::vector instead of \a std::unordered_set because vectors can be represented as contiguous memory blocks.
std::vector<uint64_t> proximity_intersection(uint64_t radius, const std::vector<uint64_t> &as, const std::vector<uint64_t> &bs) {
    std::vector<uint64_t> matches;
    // In the "worst" case, all elements of `bs` are included in the set, reserving it upfront and
    // shrinking the vector afterwards should be faster than possibly multiple allocations within the loop.
    matches.reserve(bs.size());
    for (const auto b : bs) {
        auto end = std::ranges::upper_bound(as, b + radius);
        auto begin = std::ranges::lower_bound(std::ranges::begin(as), end, b - radius);
        if (!std::ranges::empty(std::ranges::subrange(begin, end))) {
            matches.emplace_back(b);
        }
    }
    matches.shrink_to_fit();
    return matches;
}

auto read_compressed_set(fs::path path) {
    io::memory_mapped_file file(path);
    std::vector<uint64_t> result;
    // As vbyte-encoded values are one byte at minimum, the theoretical maximum count of values is
    // equal to the length of the compressed file.
    result.reserve(file.size());
    auto it = file.begin();
    while (it != file.end()) {
        result.emplace_back(vbyte::decode(&it));
    }
    result.shrink_to_fit();
    std::ranges::sort(result);
    return result;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: " << getprogname() << " radius file" << EOL;
        exit(EXIT_FAILURE);
    }

    int radius;
    try {
        radius = std::stoi(argv[1]);
    } catch (std::invalid_argument &ex) {
        error("radius must be a number");
        exit(EXIT_FAILURE);
    } catch (std::exception &ex) {
        error(ex);
        exit(EXIT_FAILURE);
    }

    fs::path inpath(argv[2]);

    try {
        auto pairs = read_index_file(inpath);
        auto start_time = std::chrono::steady_clock::now();
        std::vector<size_t> sizes;
        sizes.reserve(pairs.size());

        auto files_path = std::filesystem::path("files");

        for (const auto &pair : pairs) {
            auto set_a_filename = fs::path("F" + std::to_string(pair.first) + ".vb");
            auto set_b_filename = fs::path("F" + std::to_string(pair.second) + ".vb");
            auto set_a = read_compressed_set(files_path / set_a_filename);
            auto set_b = read_compressed_set(files_path / set_b_filename);
            auto result = proximity_intersection(radius, set_a, set_b);
            sizes.emplace_back(result.size());
        }
        for (const auto size : sizes) {
            std::cout << size << EOL;
        }
        std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start_time;
        std::cout << "Total duration: " << elapsed.count() << "ms" << EOL;
        return EXIT_SUCCESS;
    } catch (std::exception &ex) {
        error(ex);
        exit(EXIT_FAILURE);
    }
}
