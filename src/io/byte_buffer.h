#pragma once

#include "file_writer.h"
#include "../utils.h"
#include <memory>
#include <bit>
#include <span>
#include <cassert>

namespace io {
    class byte_buffer {
        friend class file_writer;
    public:
        explicit byte_buffer(size_t size): pos_(0), size_(size) {
            // Assume that sizeof(std::byte) == 1
            ptr_ = static_cast<std::byte*>(aligned_alloc(pagesize, size));
        }

        ~byte_buffer() noexcept {
            std::free(ptr_);
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return size_;
        }

        [[nodiscard]] constexpr size_t size() const noexcept {
            return pos_;
        }

        constexpr auto begin() noexcept {
            return as_span().begin();
        }

        constexpr auto end() noexcept {
            return as_span().subspan(0, pos_).end();
        }

        size_t extend(std::span<const std::byte> data) noexcept {
            auto to_write = std::min(capacity() - size(), data.size_bytes());
            std::memmove(ptr_ + pos_, data.data(), to_write);
            pos_ += to_write;
            return to_write;
        }

        void reset() noexcept {
            pos_ = 0;
        }

        [[nodiscard]] constexpr auto data() const noexcept {
            return as_span().data();
        }
    private:
        std::byte* ptr_;
        size_t pos_;
        size_t size_;

        [[nodiscard]] constexpr std::span<std::byte> as_span() const noexcept {
            return { ptr_, pos_ };
        }

        [[nodiscard]] constexpr std::byte* next() noexcept {
            return ptr_ + pos_;
        }
    };
}