#pragma once

#include "types.h"

#include "termcolor.hpp"
#undef ERROR // wingdi.h

#include <format>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>

#define LOG_ENABLE_PRINT

#ifndef LOG_LINE_LEN
#define LOG_LINE_LEN 127
#endif

#ifndef LOG_BUF_SIZE
#define LOG_BUF_SIZE 20
#endif

#define LOG_BASIC(...) ::Nexus::Log::Basic<__FILE__>(__VA_ARGS__)
#define LOG_EVENT(...) ::Nexus::Log::Event<__FILE__>(__VA_ARGS__)
#define LOG_ALERT(...) ::Nexus::Log::Alert<__FILE__>(__VA_ARGS__)
#define LOG_ERROR(...) ::Nexus::Log::Error<__FILE__>(__VA_ARGS__)

namespace Nexus
{
    struct Log
    {
        ///
        /// @brief Severity or level
        ///
        enum Type : std::uint8_t
        {
            BASIC,
            EVENT,
            ALERT,
            ERROR
        };

        ///
        /// @brief A string message and tag of `Nexus::Log::Type`
        ///
        struct Entry
        {
            char Message[LOG_LINE_LEN] = {};
            Type Tag = Type::BASIC;
        };

        template <MetaString FILE, typename... Args>
        static void Basic(const std::format_string<Args...> fmt, Args &&...args)
        {
            Log::Impl<Log::ParseFileToName<FILE>(), Type::BASIC>(fmt, std::forward<Args>(args)...);
        }

        template <MetaString FILE, typename... Args>
        static void Event(const std::format_string<Args...> fmt, Args &&...args)
        {
            Log::Impl<Log::ParseFileToName<FILE>(), Type::EVENT>(fmt, std::forward<Args>(args)...);
        }

        template <MetaString FILE, typename... Args>
        static void Alert(const std::format_string<Args...> fmt, Args &&...args)
        {
            Log::Impl<Log::ParseFileToName<FILE>(), Type::ALERT>(fmt, std::forward<Args>(args)...);
        }

        template <MetaString FILE, typename... Args>
        static void Error(const std::format_string<Args...> fmt, Args &&...args)
        {
            Log::Impl<Log::ParseFileToName<FILE>(), Type::ERROR>(fmt, std::forward<Args>(args)...);
        }

        template <MetaString NAME, Type TYPE, typename... Args>
        static void Impl(const std::format_string<Args...> fmt, Args &&...args)
        {
            auto add_null_char = [&](auto &res, auto &log)
            {
                if (res.size < LOG_LINE_LEN)
                {
                    log[res.size] = '\0';
                }
            };

            const std::string msg = std::format(fmt, std::forward<Args>(args)...);

            auto [entry, lock] = Log::Book.page();
            auto &log = entry.Message;
            entry.Tag = TYPE;

            if constexpr (TYPE == Type::BASIC)
            {
                auto res = std::format_to_n(log, FMT_SIZE, "[BASIC] ({}) : {}", NAME.String, msg);
                add_null_char(res, log);
                std::cout << termcolor::white << log << termcolor::reset << std::endl;
            }
            if constexpr (TYPE == Type::EVENT)
            {
                auto res = std::format_to_n(log, FMT_SIZE, "[EVENT] ({}) : {}", NAME.String, msg);
                add_null_char(res, log);
                std::cout << termcolor::green << log << termcolor::reset << std::endl;
            }
            if constexpr (TYPE == Type::ALERT)
            {
                auto res = std::format_to_n(log, FMT_SIZE, "[ALERT] ({}) : {}", NAME.String, msg);
                add_null_char(res, log);
                std::cerr << termcolor::yellow << log << termcolor::reset << std::endl;
            }
            if constexpr (TYPE == Type::ERROR)
            {
                auto res = std::format_to_n(log, FMT_SIZE, "[ERROR] ({}) : {}", NAME.String, msg);
                add_null_char(res, log);
                std::cerr << termcolor::red << log << termcolor::reset << std::endl;
            }
            Log::Book.next();
        }

        template <MetaString FILE>
        static consteval auto ParseFileToName()
        {
            MetaString<FILE.size()> name;

            // MSVC: absolute path
            constexpr const auto offset = 21ull;
            for (auto i = offset; i < FILE.size(); ++i)
            {
                if (FILE[i] == '\\')
                {
                    name[i - offset] = '.';
                    continue;
                }
                if (FILE[i] == '.')
                {
                    name[i - offset] = '\0';
                    break;
                }
                name[i - offset] = FILE[i];
            }
            return name;
        }

        static inline CircularBuffer<Entry, LOG_BUF_SIZE, std::mutex> Book;

        static constexpr const std::size_t FMT_SIZE = LOG_LINE_LEN - 1;
    };
}