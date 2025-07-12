#pragma once

#include "termcolor.hpp"

#include <atomic>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <thread>

namespace dt
{
    ///
    /// @brief
    ///
    class log
    {
    public:
        ///
        /// @brief
        ///
        enum class Type
        {
            Debug,
            Event,
            Alert,
            Error
        };

        ///
        /// @brief
        ///
        struct Entry
        {
            Type Type;
            std::string Message;
        };

        ///
        /// @brief
        /// @tparam N
        ///
        template <size_t N>
        class CircularBuffer
        {
        public:
            ///
            /// @brief
            ///
            class Iterator
            {
            public:
                using pointer = Entry *;
                using reference = Entry &;
                using value_type = Entry;
                using difference_type = std::ptrdiff_t;
                using iterator_category = std::input_iterator_tag;

                // Iterator(std::mutex *m, pointer p, int i) : _Count(N - i), _Ptr(p + i), _M(*m) {}
                Iterator(std::mutex *m, pointer p, int i) : M_Count(i + 1), M_Ptr(p + i - 1), M_Guard(*m) {}

                ~Iterator() { log::M_Busy = false; }

                Iterator &operator++() noexcept
                {
                    if (--M_Count == 0)
                        M_Ptr += N;

                    M_Ptr--;
                    return *this;
                }

                Iterator operator++(int) noexcept
                {
                    Iterator tmp = *this;
                    *this--;
                    return tmp;
                }

                bool operator!=(pointer p) const { return M_Count > 0 || M_Ptr != p - 1; }

                const pointer operator->() const { return M_Ptr; }

                const reference operator*() const { return *M_Ptr; }

            private:
                int M_Count;
                pointer M_Ptr;
                std::lock_guard<std::mutex> M_Guard;
            };

            void operator<<(Entry &&entry)
            {
                std::lock_guard guard(M_Mutex);

                switch (entry.Type)
                {
                case Type::Debug:
                    // NOTE: Black on Windows terminal
                    // std::cout << termcolor::grey;
                    std::cout << termcolor::bright_grey;
                    break;
                case Type::Event:
                    std::cout << termcolor::green;
                    break;
                case Type::Alert:
                    std::cout << termcolor::yellow;
                    break;
                case Type::Error:
                    std::cout << termcolor::red;
                    break;
                }
                std::cout << entry.Message << termcolor::reset << '\n';
                M_Entries[M_Index] = std::move(entry);
                M_Index = (M_Index + 1) % N;
            }

            Iterator begin()
            {
                M_Busy = true;
                return Iterator(&M_Mutex, M_Entries, M_Index);
            }

            Iterator::pointer end()
            {
                return &M_Entries[M_Index];
            }

            constexpr size_t size() const noexcept { return N; }

        private:
            int M_Index = 0;
            Entry M_Entries[N];
        };

        ///
        /// @brief Helper for logging debug messages
        /// @tparam ...T Same as `std::format`
        ///
        template <typename... T>
        struct debug
        {
            debug(const char *f, T &&...t, const std::source_location &src_loc = std::source_location::current())
            {
                const std::string fmt = std::vformat(f, std::make_format_args(t...));
                const std::string msg = std::format("[DEBUG] ({}) : {}", src_loc.function_name(), fmt);

                while (log::M_Busy)
                    std::this_thread::yield();

                Data << Entry(Type::Debug, std::move(msg));
            }
        };

        ///
        /// @brief Helper for logging event messages
        /// @tparam ...T Same as `std::format`
        ///
        template <typename... T>
        struct event
        {
            event(const char *f, T &&...t, const std::source_location &src_loc = std::source_location::current())
            {
                const std::string fmt = std::vformat(f, std::make_format_args(t...));
                const std::string msg = std::format("[EVENT] ({}) : {}", src_loc.function_name(), fmt);

                while (log::M_Busy)
                    std::this_thread::yield();

                Data << Entry(Type::Event, std::move(msg));
            }
        };

        ///
        /// @brief Helper for logging alert messages
        /// @tparam ...T Same as `std::format`
        ///
        template <typename... T>
        struct alert
        {
            alert(const char *f, T &&...t, const std::source_location &src_loc = std::source_location::current())
            {
                const std::string fmt = std::vformat(f, std::make_format_args(t...));
                const std::string msg = std::format("[ALERT] ({}) : {}", src_loc.function_name(), fmt);

                while (log::M_Busy)
                    std::this_thread::yield();

                Data << Entry(Type::Alert, std::move(msg));
            }
        };

        ///
        /// @brief Helper for logging error messages
        /// @tparam ...T Same as `std::format`
        ///
        template <typename... T>
        struct error
        {
            error(const char *f, T &&...t, const std::source_location &src_loc = std::source_location::current())
            {
                const std::string fmt = std::vformat(f, std::make_format_args(t...));
                const std::string msg = std::format("[ERROR] ({}) : {}", src_loc.function_name(), fmt);

                while (log::M_Busy)
                    std::this_thread::yield();

                Data << Entry(Type::Error, std::move(msg));
            }
        };

        template <typename... T>
        debug(const char *, T &&...) -> debug<T...>;

        template <typename... T>
        event(const char *, T &&...) -> event<T...>;

        template <typename... T>
        alert(const char *, T &&...) -> alert<T...>;

        template <typename... T>
        error(const char *, T &&...) -> error<T...>;

        log() = delete;
        ~log() = delete;

        static inline CircularBuffer<100> Data;

    private:
        static inline std::mutex M_Mutex;
        static inline std::atomic<bool> M_Busy = false;
    };
}