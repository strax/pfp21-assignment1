#pragma once

#include <concepts>
#include <filesystem>
#include <string>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <ranges>
#include <span>

template <std::semiregular T>
struct MemoryMappedFile {

private:
    std::span<T> _span;

public:
    explicit MemoryMappedFile(std::filesystem::path &path);

    const T& operator[](int index) const;

    [[nodiscard]] size_t size() const noexcept;

    ~MemoryMappedFile();
};

template<std::semiregular T>
MemoryMappedFile<T>::MemoryMappedFile(std::filesystem::path &path) {
    auto fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        auto emsg = path.string() + ": " + strerror(errno);
        throw std::runtime_error(emsg);
    }
    // mmap requires file length
    struct stat s {};
    if (stat(path.c_str(), &s) == -1) {
        auto message = path.string() + ": " + strerror(errno);
        throw std::runtime_error(message);
    }
    auto size = s.st_size;
    // The file size must be a multiple of the type we want in order to be valid
    if (size % sizeof(T) != 0) {
        throw std::runtime_error(path.string() + ": invalid file size");
    }

    void* dest = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (dest == (void*)-1) {
        throw std::runtime_error(strerror(errno));
    }
    _span = std::span<T>(reinterpret_cast<T*>(dest), size / sizeof(T));
}

template<std::semiregular T>
MemoryMappedFile<T>::~MemoryMappedFile() {
    munmap(reinterpret_cast<void*>(_span.data()), _span.size_bytes());
}

template<std::semiregular T>
const T &MemoryMappedFile<T>::operator[](int index) const {
    assert(index < size());
    return _span[index];
}

template<std::semiregular T>
size_t MemoryMappedFile<T>::size() const noexcept {
    return _span.size();
}