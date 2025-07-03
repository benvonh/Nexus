#pragma once
#include <format>
#include <source_location>
#include <stacktrace>
#include <string>

namespace dt
{
    ///
    /// @brief Concrete base class for the Digital Twin exception
    ///
    struct viewable_exception
    {
        constexpr explicit viewable_exception(std::string &&msg) : Message(msg) {}

        constexpr operator const char *() const noexcept
        {
            return Message.c_str();
        }

        const std::string Message;
    };

    ///
    /// @brief Formats a message for an exception
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct exception : viewable_exception
    {
        constexpr exception(const char *fmt, T &&...args,
                            const std::source_location &src_loc = std::source_location::current())

            : viewable_exception(
                  std::format(
                      "{}({},{})@{}: {}\n\n{}",
                      src_loc.file_name(),
                      src_loc.line(),
                      src_loc.column(),
                      src_loc.function_name(),
                      std::vformat(fmt, std::make_format_args(args...)),
                      std::stacktrace::current()))
        {
        }
    };

    template <typename... T>
    exception(const char *, T &&...) -> exception<T...>;

    template <typename... T>
    using ex = exception<T...>;
}