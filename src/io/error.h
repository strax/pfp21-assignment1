#pragma once

#include <stdexcept>
#include <system_error>
#include <filesystem>
#include <utility>

using path = std::filesystem::path;

namespace io {
    class io_error : public std::system_error {
    private:
        path path_;
        std::string message_;
    public:
        explicit io_error(int c, path p)
        : std::system_error(c, std::generic_category()), path_(std::move(p)) {
            // NOTE: We need to construct the message here instead of `what()` because
            // `what()` returns `const char*` and we want the pointer to be valid for the lifetime
            // of the exception object
            message_ = path_.string() + ": " + code().message();
        }

        [[nodiscard]] const char* what() const noexcept override {
            return message_.c_str();
        }
    };
}