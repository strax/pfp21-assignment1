#pragma once

#include <concepts>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <ranges>
#include <span>
#include <iostream>
#include <cerrno>

#include "error.h"

#if defined(__unix__) || defined(__APPLE__)
    #include <sys/mman.h>
    #include <sys/stat.h>
#else
    #error "Unsupported platform"
#endif

namespace fs = std::filesystem;

namespace io {
    enum class advise : int {
        normal = MADV_NORMAL,
        sequential = MADV_SEQUENTIAL,
        random = MADV_RANDOM,
        willneed = MADV_WILLNEED,
        dontneed = MADV_DONTNEED
    };

    template <typename T = std::byte>
    requires(std::is_fundamental<T>::value || std::is_same<T, std::byte>::value)
    struct memory_mapped_file {

    private:
        std::span<T> _span;

    public:
        explicit memory_mapped_file(fs::path &path) {
            auto fd = open(path.c_str(), O_RDONLY);
            if (fd == -1) {
                throw io_error(errno, path);
            }
            // mmap requires file length
            struct stat s {};
            if (stat(path.c_str(), &s) == -1) {
                throw io_error(errno, path);
            }
            auto size = s.st_size;
            // The file size must be a multiple of the type we want in order to be valid
            if (size % sizeof(T) != 0) {
                throw std::runtime_error(path.string() + ": invalid file size");
            }

            void* dest = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
            // The file descriptor can be closed immediately after the memory has been mapped
            close(fd);
            if (dest == MAP_FAILED) {
                throw std::runtime_error(strerror(errno));
            }
            _span = std::span<T>(static_cast<T*>(dest), size / sizeof(T));
        }

        constexpr T& operator[](int index) const {
            assert(index < size());
            return _span[index];
        }

        void advise(advise adv) {
            madvise(static_cast<void*>(_span.data()), _span.size_bytes(), static_cast<int>(adv));
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return _span.size();
        }

        [[nodiscard]] constexpr auto begin() const noexcept {
            return _span.begin();
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return _span.end();
        }

        ~memory_mapped_file() noexcept {
            munmap(static_cast<void*>(_span.data()), _span.size_bytes());
        }
    };
}

static_assert(std::ranges::contiguous_range<io::memory_mapped_file<uint64_t>>);