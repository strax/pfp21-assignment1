#pragma once

#include <concepts>
#include <iterator>

template<std::integral T, std::output_iterator<std::byte> I>
[[maybe_unused]] void VByteEncodeInteger(T i, I& output) {
    while (true) {
        T b = i % 128;
        if (i < 128) {
            *output++ = b + 128;
            break;
        }
        *output++ = b;
        i = i / 128;
    }
}