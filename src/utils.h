#pragma once

#include <exception>
#include <cstdio>
#include <unistd.h>

// Used instead of std::endl, see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rio-endl
constexpr char EOL = '\n';

#if defined(__APPLE__) || defined(__FreeBSD__)
// I first used this API to get the program name on Mac, thinking that it is standard C...
// when it didn't compile on Linux I had to shim it.
extern "C" const char* getprogname();
#elif defined(_GNU_SOURCE)
static inline const char* getprogname() noexcept {
    return program_invocation_short_name;
}
#endif

void error(const char* message) {
    std::cerr << getprogname() << ": " << message << EOL;
}

void error(std::exception &exception) {
    error(exception.what());
}

static size_t pagesize = sysconf(_SC_PAGESIZE);