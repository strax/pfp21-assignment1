#pragma once

#include <concepts>
#include <iterator>

template<std::output_iterator<char> I, std::integral T>
void VByteEncodeInteger(I& output, T i) {
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