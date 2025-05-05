#pragma once
// https://github.com/microsoft/STL/issues/3063
#define _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION 0
#include <atomic>
#include <exception>
#include <format>
#include <mutex>
#include <source_location>
#include <stacktrace>
#include <string>

namespace std
{
  using src = std::source_location;
}

namespace dt
{
  ///
  /// @brief Digital Twin exception
  /// @tparam ...T See `std::format`
  ///
  /// This exception formats the arguments from the constructor
  /// into a string. The source location and stack trace of the
  /// call site are also included.
  ///
  template <typename... T>
  struct exception : public std::runtime_error
  {
    exception(const char *fmt, T &&...args, const std::src &src = std::src::current())
        : std::runtime_error(std::format("{}({},{})@{}: {}\n{}",
                                         src.file_name(),
                                         src.line(),
                                         src.column(),
                                         src.function_name(),
                                         std::vformat(fmt, std::make_format_args(args...)),
                                         std::stacktrace::current())) {}
  };

  template <typename... T>
  exception(const char *, T &&...) -> exception<T...>;

  constexpr size_t SIZE = 8;

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

        Iterator(std::mutex &m, pointer p, int i) : left(N - i), ptr(p + i), lg(m) {}

        ~Iterator() { log::read = false; }

        Iterator &operator++()
        {
          if (--this->left == 0)
          {
            this->ptr -= N;
          }

          this->ptr++;
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
          return this->left > 0 || this->ptr != p;
        }

        const pointer operator->() const { return this->ptr; }
        const reference operator*() const { return *this->ptr; }

      private:
        int left;
        pointer ptr;
        std::lock_guard<std::mutex> lg;
      };

      void operator<<(Entry &&entry)
      {
        std::lock_guard _(log::mutex);
        this->entry[this->index] = entry;
        this->index = (this->index + 1) % N;
      }

      Iterator begin()
      {
        log::read = true;
        return Iterator(log::mutex, this->entry, this->index);
      }

      Iterator::pointer end()
      {
        return &this->entry[this->index];
      }

      ///
      /// @return Number of entries (compile-time constant)
      ///
      constexpr int size() const noexcept { return N; }

    private:
      int index = 0;
      Entry entry[N];
    }; // class Data

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
    /// @brief Helper for logging events
    /// @tparam ...T Same as in `std::format`
    ///
    template <typename... T>
    struct event
    {
      event(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[EVENT] ({}) : {}", src.function_name(), fmt);

        while (log::read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Event, std::move(msg)};
      }
    };

    ///
    /// @brief Helper for logging alerts
    /// @tparam ...T Same as in `std::format`
    ///
    template <typename... T>
    struct alert
    {
      alert(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[ALERT] ({}) : {}", src.function_name(), fmt);

        while (log::read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Alert, std::move(msg)};
      }
    };

    ///
    /// @brief Helper for logging errors
    /// @tparam ...T Same as in `std::format`
    ///
    template <typename... T>
    struct error
    {
      error(const char *format, T &&...args, const std::src &src = std::src::current())
      {
        const std::string fmt = std::vformat(format, std::make_format_args(args...));
        const std::string msg = std::format("[ERROR] ({}) : {}", src.function_name(), fmt);

        while (log::read)
        {
          std::this_thread::yield();
        }

        log::data << log::Entry{log::Type::Error, std::move(msg)};
      }
    };

    template <typename... T>
    event(const char *, T &&...) -> event<T...>;

    template <typename... T>
    alert(const char *, T &&...) -> alert<T...>;

    template <typename... T>
    error(const char *, T &&...) -> error<T...>;

  private:
    static inline std::mutex mutex;
    static inline std::atomic<bool> read{};
  }; // class log
}