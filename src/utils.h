#pragma once

#include <exception>
#include <cstdio>

#if defined(__APPLE__) || defined(__FreeBSD__)
extern "C" const char* getprogname();
#elif defined(_GNU_SOURCE)
static inline const char* getprogname() noexcept {
    return program_invocation_short_name;
}
#endif

void error(const char* message) {
    fprintf(stderr, "%s: %s\n", getprogname(), message);
}

void error(std::exception &exception) {
    error(exception.what());
}