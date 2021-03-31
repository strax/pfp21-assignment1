#pragma once

#include <exception>
#include <cstdio>

void error(const char* message) {
    fprintf(stderr, "%s: %s\n", getprogname(), message);
}

void error(std::exception &exception) {
    error(exception.what());
}