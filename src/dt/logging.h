#pragma once
// https://github.com/microsoft/STL/issues/3063
#define _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION 0
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
        class Data
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

                Iterator(std::mutex *m, pointer p, int i) : _Count(N - i), _Ptr(p + i), _M(*m) {}

                ~Iterator() { log::_Busy = false; }

                Iterator &operator++()
                {
                    if (--_Count == 0)
                        _Ptr -= N;

                    _Ptr++;
                    return *this;
                }

                Iterator operator++(int)
                {
                    Iterator tmp = *this;
                    *this ++;
                    return tmp;
                }

                bool operator!=(pointer p) const { return _Count > 0 || _Ptr != p; }
                const pointer operator->() const { return _Ptr; }
                const reference operator*() const { return *_Ptr; }

            private:
                int _Count;
                pointer _Ptr;
                std::lock_guard<std::mutex> _M;
            };

            void operator<<(Entry &&entry)
            {
                std::lock_guard guard(_Mutex);

                switch (entry.Type)
                {
                case Type::Debug:
                    std::cout << termcolor::grey;
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
                std::cout << entry.Message << termcolor::reset << std::endl;
                _Entries[_Index] = std::move(entry);
                _Index = (_Index + 1) % N;
            }

            Iterator begin()
            {
                _Busy = true;
                return Iterator(&_Mutex, _Entries, _Index);
            }

            Iterator::pointer end()
            {
                return &_Entries[_Index];
            }

            constexpr size_t size() const noexcept { return N; }

        private:
            int _Index = 0;
            Entry _Entries[N];
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

                while (log::_Busy)
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

                while (log::_Busy)
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

                while (log::_Busy)
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

                while (log::_Busy)
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
        log(log &&) = delete;
        log(const log &) = delete;
        log &operator=(log &&) = delete;
        log &operator=(const log &) = delete;

        static inline Data<100> Data;

    private:
        static inline std::mutex _Mutex;
        static inline std::atomic<bool> _Busy = false;
    };
}