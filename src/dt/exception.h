#pragma once

#include <format>
#include <source_location>
#include <stacktrace>
#include <string>

#define ASSERT(expr)                \
    do                              \
    {                               \
        if (!(expr)) [[unlikely]]   \
            throw exception(#expr); \
    } while (0)

#define CHECK(expr, except)          \
    do                               \
    {                                \
        if (!(expr)) [[unlikely]]    \
            throw except(ex(#expr)); \
    } while (0)

namespace dt
{
    ///
    /// @brief Concrete exception
    ///
    struct Exception
    {
        constexpr Exception(std::string &&msg) : Message(msg) {}

        constexpr operator const char *() const noexcept
        {
            return Message.c_str();
        }

        constexpr const char *what() const noexcept
        {
            return Message.c_str();
        }

        const std::string Message;
    };

    ///
    /// @brief Digital Twin exception
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct exception : Exception
    {
        constexpr exception(const char *fmt, T &&...args,
                            const std::source_location &srcloc = std::source_location::current())

            : Exception(
                  std::format(
                      "{}({},{})@{}: {}\n\n{}",
                      srcloc.file_name(),
                      srcloc.line(),
                      srcloc.column(),
                      srcloc.function_name(),
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