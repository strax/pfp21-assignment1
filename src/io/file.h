#pragma once

#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <utility>
#include <ranges>
#include <unistd.h>
#include <sys/uio.h>

#include "error.h"
#include "buffer.h"
#include "../utils.h"

using path = std::filesystem::path;
namespace ranges = std::ranges;

namespace io {
    static size_t page_size = system_page_size();

    enum class mode : uint8_t {
        read = 0x01,
        write = 0x02
    };

    class file {
    public:
        file(path p, mode m): path_(std::move(p)), mode_(m), write_buffer_(buffer(page_size)) {
            fd_ = open(path_.c_str(), mode_ == mode::write ? O_RDWR | O_CREAT | O_TRUNC : O_RDONLY);
            if (!fd_) {
                throw io_error(errno, path_);
            }
        }

//        void write(ranges::contiguous_range auto&& range)
//        requires std::is_same<ranges::range_value_t<decltype(range)>, std::byte>::value {
//            ranges::move(range, write_buffer_.end());
//        }

        template <ranges::contiguous_range R>
        requires std::is_same<ranges::range_value_t<R>, std::byte>::value
        void write(R&& range) {
            return write(std::span{range});
        }

        void write(std::span<std::byte> data) {
            auto count = write_buffer_.capacity() - write_buffer_.size();
            if (data.size() < count) {
                write_buffer_.extend(data);
            } else {
                std::array<struct iovec, 2> iov{};
                iov[0].iov_base = write_buffer_.data();
                iov[0].iov_len = write_buffer_.size();
                iov[1].iov_base = data.data();
                iov[1].iov_len = data.size();
                auto written = ::writev(fd_, iov.data(), 2);
                if (written < 0) {
                    throw io_error(errno, path_);
                }

                write_buffer_.reset();
            }
        }

        ~file() noexcept {
            close(fd_);
        }

    private:
        path path_;
        mode mode_;
        int fd_;
        buffer write_buffer_;

        void commit() {
            auto written = ::write(fd_, static_cast<const void*>(write_buffer_.data()), write_buffer_.size());
            if (written < 0) {
                throw io_error(errno, path_);
            }
            write_buffer_.reset();
        }
    };
}