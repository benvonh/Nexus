#pragma once

#include <format>
#include <source_location>
#include <stacktrace>
#include <string>

#define ASSERT(expr)                         \
    do                                       \
    {                                        \
        if (!(expr)) [[unlikely]]            \
            throw ::Nexus::exception(#expr); \
    } while (0)

#define EXPLAIN(...) ::Nexus::exception(__VA_ARGS__)

namespace Nexus
{
    ///
    /// @brief A custom exception
    ///
    struct Exception
    {
        Exception(std::string &&msg) : Message(std::move(msg))
        {
        }

        [[nodiscard]]
        operator const char *() const
        {
            return Message.c_str();
        }

        [[nodiscard]]
        const char *what() const
        {
            return Message.c_str();
        }

        const std::string Message;
    };

    ///
    /// @brief A detailed exception with formatting
    /// @tparam ...Args Forwarded to `std::format`
    ///
    template <typename... Args>
    struct exception : Exception
    {
        exception(const std::format_string<Args...> fmt, Args &&...args,
                  const std::stacktrace trace = std::stacktrace::current(),
                  const std::source_location loc = std::source_location::current())

            : Exception(std::format("{}({},{}) @ {}: {}\n\n{}",
                                  loc.file_name(),
                                  loc.line(),
                                  loc.column(),
                                  loc.function_name(),
                                  std::format(fmt, std::forward<Args>(args)...),
                                  trace))
        {
        }
    };

    template <typename... T>
    exception(const std::format_string<T...>, T &&...) -> exception<T...>;
}