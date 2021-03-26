#pragma once

#include <concepts>
#include <iterator>
#include <bit>
#include <cstddef>

namespace vbyte {
    void encode(std::output_iterator<uint8_t> auto&& output, uint64_t i) {
        while (true) {
            uint64_t b = i % 128;
            if (i < 128) {
                *output++ = b + 128;
                break;
            }
            *output++ = b;
            i = i / 128;
        }
    }

    uint64_t decode(std::input_iterator auto&& input)
    requires(std::convertible_to<std::iter_value_t<decltype(*input)>, uint8_t> || std::is_same_v<std::iter_value_t<decltype(*input)>, std::byte>) {
        uint64_t i = 0;
        uint64_t p = 1;
        while (true) {
            uint8_t b;
            auto c = **input++;
            if constexpr (std::is_same<decltype(c), std::byte>::value) {
                b = std::to_integer<uint8_t>(c);
            } else {
                b = static_cast<uint8_t>(c);
            }
            if (b >= 128) {
                i += (b - 128) * p;
                break;
            }
            i += b * p;
            p *= 128;
        }
        return i;
    }
}