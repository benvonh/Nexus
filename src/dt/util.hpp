#pragma once
// https://github.com/microsoft/STL/issues/3063
#define _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION 0
#include "termcolor.hpp"
#include <atomic>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <stacktrace>
#include <stdexcept>
#include <string>

namespace std
{
  using src = std::source_location;
}

namespace dt
{
  template <typename... T>
  struct format_exception
  {
    const std::string string;

    format_exception(const char *fmt, T &&...args, const std::src &src = std::src::current())
        : string(std::format("{}({},{})@{}: {}\n{}",
                             src.file_name(),
                             src.line(),
                             src.column(),
                             src.function_name(),
                             std::vformat(fmt, std::make_format_args(args...)),
                             std::stacktrace::current()))
    {
    }

    constexpr operator const char *() const noexcept
    {
      return string.c_str();
    }
  };

  template <typename... T>
  format_exception(const char *, T &&...) -> format_exception<T...>;

  template <typename... T>
  using exfmt = format_exception<T...>;

  ///
  /// @brief Non-templated custom exception
  /// @note DO NOT USE
  ///
  /// This exception is used as a core for the implemented version
  /// so that it may be caught without the trouble of template deduction.
  ///
  struct custom_exception : public std::exception
  {
  };

  ///
  /// @brief Digital Twin exception
  /// @tparam ...T See `std::format`
  ///
  /// This exception formats the arguments from the constructor
  /// into a string. The source location and stack trace of the
  /// call site are also included.
  ///
  template <typename... T>
  struct exception : public custom_exception
  {
    exception(const char *fmt, T &&...args, const std::src &src = std::src::current())
        : std::exception(format_exception<T...>(fmt, std::forward<T>(args)..., src))
    // This is non-standard by MSVC
    {
    }
  };

  template <typename... T>
  exception(const char *, T &&...) -> exception<T...>;

  constexpr size_t SIZE = 32;

  ///
  /// @brief Digital Twin log
  ///
  class log
  {
  public:
    ///
    /// @note `ERROR` is an identifier in MSVC
    /// so the enumeration of `Type` are in lowercase.
    ///
    enum class Type
    {
      Debug,
      Event,
      Alert,
      Error
    };

    ///
    /// @brief Basic log structure of some type and message
    ///
    struct Entry
    {
      Type type;
      std::string message;
    };

    ///
    /// @brief Log entries as a circular buffer
    /// @tparam N Number of fixed-size entries
    ///
    template <int N>
    class Data
    {
    public:
      ///
      /// @brief Iterator for log entries
      ///
      class Iterator
      {
      public:
        using pointer = Entry *;
        using reference = Entry &;
        using value_type = Entry;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        Iterator(std::mutex &m, pointer p, int i) : _left(N - i), _ptr(p + i), _lg(m) {}

        ~Iterator() { log::_read = false; }

        Iterator &operator++()
        {
          if (--_left == 0)
          {
            _ptr -= N;
          }

          _ptr++;
          return *this;
        }

        Iterator operator++(int)
        {
          Iterator tmp = *this;
          ++(*this);
          return tmp;
        }

        bool operator!=(pointer p) const
        {
          return _left > 0 || _ptr != p;
        }

        const pointer operator->() const { return _ptr; }
        const reference operator*() const { return *_ptr; }

      private:
        int _left;
        pointer _ptr;
        std::lock_guard<std::mutex> _lg;
      };

      void operator<<(Entry &&entry)
      {
        std::lock_guard _(log::_mutex);
        entry[_index] = entry;
        _index = (_index + 1) % N;

        switch (entry.type)
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
        std::cout << entry.message << termcolor::reset << std::endl;
      }

      Iterator begin()
      {
        _read = true;
        return Iterator(_mutex, _entry, _index);
      }

      Iterator::pointer end()
      {
        return &_entry[_index];
      }

      ///
      /// @return Log entry buffer size
      ///
      constexpr int size() const noexcept { return N; }

    private:
      int _index = 0;
      Entry _entry[N];
    };

    ///
    /// Global log data
    ///
    static inline Data<SIZE> data;

    log() = delete;
    ~log() = delete;
    log(log &&) = delete;
    log(const log &) = delete;
    log &operator=(log &&) = delete;
    log &operator=(const log &) = delete;

    ///
    /// @brief Helper for logging debug messages
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct debug
    {
      debug(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[DEBUG] ({}) : {}", src.function_name(), fmt);

        while (log::_read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Debug, std::move(msg)};
      }
    };

    ///
    /// @brief Helper for logging event messages
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct event
    {
      event(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[EVENT] ({}) : {}", src.function_name(), fmt);

        while (log::_read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Event, std::move(msg)};
      }
    };

    ///
    /// @brief Helper for logging alert messages
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct alert
    {
      alert(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[ALERT] ({}) : {}", src.function_name(), fmt);

        while (log::_read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Alert, std::move(msg)};
      }
    };

    ///
    /// @brief Helper for logging error messages
    /// @tparam ...T Same as `std::format`
    ///
    template <typename... T>
    struct error
    {
      error(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[ERROR] ({}) : {}", src.function_name(), fmt);

        while (log::_read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Error, std::move(msg)};
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

  private:
    static inline std::mutex _mutex;
    static inline std::atomic<bool> _read = false;
  };
}