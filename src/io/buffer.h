#pragma once

#include <memory>
#include <bit>
#include <span>
#include <cassert>

namespace io {
    class buffer {
    public:
        explicit buffer(size_t size): count_(0), size_(size) {
            ptr_ = std::malloc(size);
        }

        ~buffer() noexcept {
            std::free(ptr_);
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return size_;
        }

        [[nodiscard]] size_t size() const noexcept {
            return count_;
        }

        constexpr auto begin() noexcept {
            return as_span().begin();
        }

        constexpr auto end() noexcept {
            return as_span().subspan(0, count_).end();
        }

        void extend(std::span<std::byte> data) noexcept {
            assert(capacity() - size() >= data.size_bytes());
            auto dest = as_span().subspan(count_);
            std::memcpy(dest.data(), data.data(), data.size_bytes());
            count_ += data.size_bytes();
        }

        void reset() noexcept {
            count_ = 0;
        }

        [[nodiscard]] constexpr auto data() const noexcept {
            return as_span().data();
        }
    private:
        void* ptr_;
        size_t count_;
        size_t size_;

        [[nodiscard]] constexpr std::span<std::byte> as_span() const noexcept {
            return { static_cast<std::byte*>(ptr_), size_ };
        }
    };
}