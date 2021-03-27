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
/// Note that \a as is passed by value since it is sorted in-place whereas \a bs is passed as a const reference.
/// This way the caller can decide whether to move \a as to this function or copy the vector beforehand.
/// We use \a std::vector instead of \a std::unordered_set because vectors can be represented as contiguous memory blocks.
std::vector<uint64_t> proximity_intersection(uint64_t radius, std::vector<uint64_t> as, const std::vector<uint64_t> &bs) {
    std::ranges::sort(as);
    std::cerr << "proximity_intersection: sorted as" << std::endl;
    std::vector<uint64_t> matches;
    // In the "worst" case, all elements are included in the intersection
    matches.reserve(std::max(as.size(), bs.size()));
    for (const auto b : bs) {
        auto end = std::ranges::upper_bound(as, b + radius);
        auto begin = std::ranges::lower_bound(std::ranges::begin(as), end, b - radius);
        if (!std::ranges::empty(std::ranges::subrange(begin, end))) {
            matches.emplace_back(b);
        }
//        for (const auto a : as) {
//            uint64_t delta = (a > b) ? a - b : b - a;
//            // If b is in range [a-z,a+z] then add it to results.
//            // We can move on to the next element since this one has been added
//            if (delta <= radius) {
//                matches.emplace_back(b);
//                break;
//            }
//            // Since `as` is sorted, we can stop iterating when we have exceeded the upper bound (a+z)
//            // as at that point every remaining element in the sequence is guaranteed to be more than that
//            else if (a > b + radius) {
//                break;
//            }
//        }
    }
    matches.shrink_to_fit();
    return matches;
}

std::vector<uint64_t> read_compressed_set(fs::path path) {
    io::memory_mapped_file file(path);
    std::cerr << "file.size() == " << file.size() << std::endl;
    std::vector<uint64_t> result;
    auto it = file.begin();
    std::cerr << std::to_address(it) << " " << std::to_address(file.end()) << std::endl;
    while (it != file.end()) {
        result.emplace_back(vbyte::decode(&it));
    }
    return result;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <radius> <filename>" << std::endl;
        exit(EX_USAGE);
    }

    try {
        int radius = std::stoi(std::string(argv[1]));
        std::cerr << "radius: " << std::to_string(radius) << std::endl;
        fs::path inpath(argv[2]);

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
            auto result = proximity_intersection(radius, std::move(set_a), set_b);
            sizes.emplace_back(result.size());
        }
        for (const auto size : sizes) {
            std::cout << size << std::endl;
        }
        std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start_time;
        std::cout << "Total duration: " << elapsed << "ms" << std::endl;
        return EX_OK;
    } catch (std::exception &ex) {
        std::cerr << PROGRAM_NAME << ": " << ex.what() << std::endl;
    }
}
