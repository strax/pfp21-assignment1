#pragma once

#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <utility>
#include <ranges>
#include <unistd.h>
#include <sys/uio.h>

#include "error.h"

using path = std::filesystem::path;
namespace ranges = std::ranges;

namespace io {
    constexpr size_t buffer_size = 4096;

    class file {
        enum class mode : uint8_t {
            read = 0x01,
            write = 0x02
        };

        file(path p, mode m): path_(std::move(p)), mode_(m) {
            fd_ = open(path_.c_str(), mode_ == mode::write ? O_RDWR | O_CREAT | O_TRUNC : O_RDONLY);
            if (!fd_) {
                throw io_error(errno, path_);
            }
            write_buffer_.reserve(buffer_size);
        }

        void write(ranges::contiguous_range auto&& range)
        requires std::is_same<ranges::range_value_t<decltype(range)>, std::byte>::value {
            ranges::move(range, write_buffer_.end());
        }

        ~file() noexcept {
            close(fd_);
        }

    private:
        path path_;
        mode mode_;
        int fd_;
        std::vector<std::byte> write_buffer_;
    };
}