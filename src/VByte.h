#pragma once

#include <concepts>
#include <iterator>

template<std::output_iterator<unsigned char> It>
void VByteEncodeInteger(It output, uint64_t i) {
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

template<std::input_iterator It>
uint64_t VByteDecodeInteger(It input) {
    uint64_t i = 0;
    uint64_t p = 1;
    while (true) {
        unsigned char b = *input++;
        if (b >= 128) {
            i += (b - 128) * p;
            break;
        }
        i += b * p;
        p *= 128;
    }
    return i;
}