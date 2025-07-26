#pragma once

#include "types.h"

#include "termcolor.hpp"
#undef ERROR // wingdi.h

#include <format>
#include <iostream>
#include <mutex>

#define LOG_ENABLE_PRINT

// It is better for alignment to set the line length
// to 2^n - 1  because `Nexus::Log::Entry` contains
// just one byte for `Nexus::Log::Type`.
#ifndef LOG_LINE_LEN
#define LOG_LINE_LEN 127
#endif

#ifndef LOG_BUF_SIZE
#define LOG_BUF_SIZE 20
#endif

#define LOGGER(class) ::Nexus::Logger<#class>

#define LOG_BASIC(...) Log<::Nexus::Log::Type::BASIC>(__VA_ARGS__)
#define LOG_EVENT(...) Log<::Nexus::Log::Type::EVENT>(__VA_ARGS__)
#define LOG_ALERT(...) Log<::Nexus::Log::Type::ALERT>(__VA_ARGS__)
#define LOG_ERROR(...) Log<::Nexus::Log::Type::ERROR>(__VA_ARGS__)

#define LOG_BASIC_TAG(tag, ...) ::Nexus::Logger<#tag>::Log<::Nexus::Log::Type::BASIC>(__VA_ARGS__)
#define LOG_EVENT_TAG(tag, ...) ::Nexus::Logger<#tag>::Log<::Nexus::Log::Type::EVENT>(__VA_ARGS__)
#define LOG_ALERT_TAG(tag, ...) ::Nexus::Logger<#tag>::Log<::Nexus::Log::Type::ALERT>(__VA_ARGS__)
#define LOG_ERROR_TAG(tag, ...) ::Nexus::Logger<#tag>::Log<::Nexus::Log::Type::ERROR>(__VA_ARGS__)

#define GENERATE_LOG_FUNCTIONS(name)                                                              \
    template <typename... Args>                                                                   \
    inline void LOG_BASIC_##name(const std::format_string<Args...> fmt, Args &&...args)           \
    {                                                                                             \
        ::Nexus::Logger<#name>::Log<::Nexus::Log::Type::BASIC>(fmt, std::forward<Args>(args)...); \
    }                                                                                             \
    template <typename... Args>                                                                   \
    inline void LOG_EVENT_##name(const std::format_string<Args...> fmt, Args &&...args)           \
    {                                                                                             \
        ::Nexus::Logger<#name>::Log<::Nexus::Log::Type::EVENT>(fmt, std::forward<Args>(args)...); \
    }                                                                                             \
    template <typename... Args>                                                                   \
    inline void LOG_ALERT_##name(const std::format_string<Args...> fmt, Args &&...args)           \
    {                                                                                             \
        ::Nexus::Logger<#name>::Log<::Nexus::Log::Type::ALERT>(fmt, std::forward<Args>(args)...); \
    }                                                                                             \
    template <typename... Args>                                                                   \
    inline void LOG_ERROR_##name(const std::format_string<Args...> fmt, Args &&...args)           \
    {                                                                                             \
        ::Nexus::Logger<#name>::Log<::Nexus::Log::Type::ERROR>(fmt, std::forward<Args>(args)...); \
    }

namespace Nexus
{
    namespace Log
    {
        ///
        /// @brief Severity or level
        ///
        enum class Type : std::uint8_t
        {
            BASIC,
            EVENT,
            ALERT,
            ERROR
        };

        ///
        /// @brief A string message with a `Nexus::Log::Type`
        ///
        struct Entry
        {
            char Text[LOG_LINE_LEN] = {};
            Type Tag = Type::BASIC;
        };

        ///
        /// @brief A circular buffer of `Nexus::Log::Entry` as a singleton
        ///
        class Book
        {
        public:
            ///
            /// @brief Get the log book
            /// @return Circular buffer of `Nexus::Log::Entry`
            ///
            static auto &Get() noexcept { return s_Book; }

            ///
            /// @brief Get the size of the log book
            /// @return Size of the circular buffer
            ///
            static auto Size() noexcept { return s_Book.size(); }

        protected:
            static inline CircularBuffer<Entry, LOG_BUF_SIZE, std::mutex> s_Book;
        };
    }

    ///
    /// @brief A logger with a name
    /// @tparam NAME Name of logger
    ///
    template <MetaString NAME>
    class Logger : Log::Book
    {
    public:
        ///
        /// @brief Format a message of `Log::Type` and log an entry in `Log::Book`
        /// @tparam TYPE One of `Log::Type`
        /// @tparam ...Args Forwarded to `std::format`
        /// @param fmt Format string
        /// @param ...args Forwarded to `std::format`
        ///
        template <Log::Type TYPE, typename... Args>
        static void Log(const std::format_string<Args...> fmt, Args &&...args)
        {
            char msg[LOG_LINE_LEN];

            auto res = std::format_to_n(msg, FMT_SIZE, fmt, std::forward<Args>(args)...);

            if (res.size >= FMT_SIZE)
            {
                *res.out = '\0';
            }
            else
            {
                msg[res.size] = '\0';
            }

            s_Book.lock();

            if constexpr (TYPE == Log::Type::BASIC)
                res = std::format_to_n(s_Book.peek().Text, FMT_SIZE, FMT_BASIC, msg);

            if constexpr (TYPE == Log::Type::EVENT)
                res = std::format_to_n(s_Book.peek().Text, FMT_SIZE, FMT_EVENT, msg);

            if constexpr (TYPE == Log::Type::ALERT)
                res = std::format_to_n(s_Book.peek().Text, FMT_SIZE, FMT_ALERT, msg);

            if constexpr (TYPE == Log::Type::ERROR)
                res = std::format_to_n(s_Book.peek().Text, FMT_SIZE, FMT_ERROR, msg);

            if (res.size >= FMT_SIZE)
            {
                *res.out = '\0';
            }
            else
            {
                s_Book.peek().Text[res.size] = '\0';
            }

#ifdef LOG_ENABLE_PRINT
            switch (TYPE)
            {
            case Log::Type::BASIC:
                std::cout << termcolor::white << s_Book.peek().Text << termcolor::reset << std::endl;
                break;
            case Log::Type::EVENT:
                std::cout << termcolor::green << s_Book.peek().Text << termcolor::reset << std::endl;
                break;
            case Log::Type::ALERT:
                std::cerr << termcolor::yellow << s_Book.peek().Text << termcolor::reset << std::endl;
                break;
            case Log::Type::ERROR:
                std::cerr << termcolor::red << s_Book.peek().Text << termcolor::reset << std::endl;
                break;
            }
#endif

            s_Book.peek().Tag = TYPE;
            s_Book.next();
            s_Book.unlock();
        }

    private:
        static constexpr std::size_t FMT_SIZE = LOG_LINE_LEN - 1;

        static constexpr MetaString FMT_BASIC = "[BASIC] (" + NAME + ") : {}";
        static constexpr MetaString FMT_EVENT = "[EVENT] (" + NAME + ") : {}";
        static constexpr MetaString FMT_ALERT = "[ALERT] (" + NAME + ") : {}";
        static constexpr MetaString FMT_ERROR = "[ERROR] (" + NAME + ") : {}";

        static_assert(FMT_SIZE > FMT_BASIC.size(), "LOG_LINE_LEN");
        static_assert(FMT_SIZE > FMT_EVENT.size(), "LOG_LINE_LEN");
        static_assert(FMT_SIZE > FMT_ALERT.size(), "LOG_LINE_LEN");
        static_assert(FMT_SIZE > FMT_ERROR.size(), "LOG_LINE_LEN");
    };
}