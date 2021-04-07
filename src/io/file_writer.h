#pragma once

#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <utility>
#include <ranges>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#ifdef __linux__
#include <sys/file.h>
#endif

#include "error.h"
#include "byte_buffer.h"
#include "../utils.h"

using path = std::filesystem::path;
namespace ranges = std::ranges;

namespace io {
    class file_writer {
        struct iterator {
        public:
            using difference_type = ptrdiff_t;

            constexpr iterator() noexcept : file_(nullptr) {}

            explicit constexpr iterator(file_writer &f): file_(&f) {}

            constexpr iterator& operator++() noexcept {
                return *this;
            }

            constexpr iterator& operator*() noexcept {
                return *this;
            }

            constexpr iterator operator++(int) noexcept {
                return *this;
            }

            constexpr iterator& operator=(std::byte&& b) {
                file_->write(b);
                return *this;
            }
        private:
            file_writer* file_;
        };

        // static_assert(std::weakly_incrementable<iterator>);
        // static_assert(std::input_or_output_iterator<iterator>);
        // static_assert(std::indirectly_writable<iterator, std::byte>);
        static_assert(std::output_iterator<iterator, std::byte>);


    public:
        explicit file_writer(path p): path_(std::move(p)), write_buffer_(byte_buffer(pagesize)) {
            // Open a new file_writer descriptor in write mode (O_WRONLY).
            // * The file_writer is emptied (O_TRUNC) on opening, and if it does not exist, it is created (O_CREAT).
            // * An exclusive lock is placed during opening the file_writer, otherwise a shared (advisory) lock is placed.
            //
            // The permissions for the created file_writer correspond to the default permissions used by `touch`, see
            // https://pubs.opengroup.org/onlinepubs/9699919799/utilities/touch.html
            auto oflags = O_WRONLY | O_CREAT | O_TRUNC;
#ifdef __APPLE__
            // O_EXLOCK does not exist on Linux
            oflags |= O_EXLOCK;
#endif
            fd_ = open(path_.c_str(), oflags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            if (!fd_) {
                throw io_error("open", errno, path_);
            }
#ifdef __linux__
            // Lock the file explicitly since we could not set O_EXLOCK
            if (flock(fd_, LOCK_EX) == -1) {
                throw io_error("flock", errno, path_);
            }
#endif
        }

        void write(std::span<const std::byte> span) {
            if (write_buffer_.size() == 0 && span.size_bytes() >= write_buffer_.capacity()) {
                // Optimization: bypass byte_buffer and write directly to disk
                _write({ span });
            } else if (span.size_bytes() >= write_buffer_.capacity() - write_buffer_.size()) {
                // Since the output is larger than available space in byte_buffer, avoid copying to byte_buffer by writing
                // byte_buffer and input
                _write({ write_buffer_.as_span(), span });
                write_buffer_.reset();
            } else {
                auto ncopied = write_buffer_.extend(span);
                assert(ncopied == span.size_bytes());
            }
        }

        template <typename T>
        requires std::is_trivially_copyable<T>::value
        constexpr void write(std::span<T> data) {
            write(std::as_bytes(data));
        }

        template <typename T>
        requires std::is_trivially_copyable<T>::value
        constexpr void write(T value) {
            std::span sp(std::addressof(value), std::addressof(value) + 1);
            write(sp);
        }

        [[nodiscard]] constexpr auto buffer_size() const noexcept {
            return write_buffer_.capacity();
        }

        [[nodiscard]] constexpr iterator begin() noexcept {
            return iterator(*this);
        }

        ~file_writer() noexcept {
            flush();
            fsync(fd_);
            flock(fd_, LOCK_UN);
            close(fd_);
        }

    private:
        path path_;
        int fd_;
        byte_buffer write_buffer_;
        std::vector<struct iovec> iov_;

        size_t _write(std::initializer_list<std::span<const std::byte>> buffers) {
            iov_.reserve(buffers.size());
            for (const auto& buffer : buffers) {
                iov_.emplace_back(iovec { .iov_base = (void *) buffer.data(), .iov_len = buffer.size_bytes() });
            }
            auto written = ::writev(fd_, iov_.data(), iov_.size());
            if (written < 0) {
                throw io_error("writev", errno, path_);
            }
            iov_.clear();
            return written;
        }

        void flush() {
            if (write_buffer_.size() > 0) {
                _write({ write_buffer_.as_span() });
                write_buffer_.reset();
            }
        }
    };
}