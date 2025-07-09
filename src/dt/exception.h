#pragma once

#include <format>
#include <source_location>
#include <stacktrace>
#include <string>

#define DT_INVOKE(__fn__, __except__, __why__) \
    do                                         \
    {                                          \
        if (!(__fn__)) [[unlikely]]            \
            throw __except__(dt::e(__why__));  \
    } while (0)

#define DT_THROW_IF(__expr__, __except__)       \
    do                                          \
    {                                           \
        if ((__expr__)) [[unlikely]]            \
            throw __except__(dt::e(#__expr__)); \
    } while (0)

namespace dt
{
    ///
    /// @brief Concrete base class for the Digital Twin exception
    ///
    struct __exception__
    {
        constexpr __exception__(std::string &&msg) : Message(msg) {}

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
    /// @brief Formats a message for an exception
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct exception : __exception__
    {
        constexpr exception(const char *fmt, T &&...args,
                            const std::source_location &src_loc = std::source_location::current())

            : __exception__(
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
    using e = exception<T...>;
}