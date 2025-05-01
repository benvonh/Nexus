#pragma once
// https://github.com/microsoft/STL/issues/3063
#define _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION 0
#include <atomic>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>

namespace dt
{
constexpr size_t SIZE = 8;
///////////////////////////////////////////////////////////////////////////////
// Static log class
///////////////////////////////////////////////////////////////////////////////
class log
{
  using loc = std::source_location;

public:
  // NOTE: 'ERROR' is already an identifier
  // so lowercase enumeration were written.
  enum class Type { Event, Alert, Error };

  struct Entry
  {
    Type type;
    std::string message;
  };

  /////////////////////////////////////////////////////////////////////////////
  // Data inner class
  /////////////////////////////////////////////////////////////////////////////
  template <int N>
  class Data
  {
  public:
    ///////////////////////////////////////////////////////////////////////////
    // Iterator for log entry
    ///////////////////////////////////////////////////////////////////////////
    class Iterator
    {
    public:
      using pointer           = Entry *;
      using reference         = Entry &;
      using value_type        = Entry;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::input_iterator_tag;

      Iterator(std::mutex &m, pointer p, int i) : left(N - i), ptr(p + i), lg(m) {}

      ~Iterator() { log::read = false; }

      Iterator &operator++()
      {
        if (--this->left == 0)
        { this->ptr -= N; }

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
        return (this->left > 0) || this->ptr != p;
      }
  
      const pointer operator->() const { return this->ptr; }
      const reference operator*() const { return *this->ptr; }

    private:
      int left;
      pointer ptr;
      std::lock_guard<std::mutex> lg;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Public log data API
    ///////////////////////////////////////////////////////////////////////////
    constexpr int size() const noexcept { return N; }

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

    Entry *end()
    {
      return &this->entry[this->index];
    }

  private:
    int index = 0;
    Entry entry[N];
  };

  /////////////////////////////////////////////////////////////////////////////
  // Global log data
  /////////////////////////////////////////////////////////////////////////////
  static inline Data<SIZE> data;

  log() = delete;
  ~log() = delete;
  log(log &&) = delete;
  log(const log &) = delete;
  log &operator=(log &&) = delete;
  log &operator=(const log &) = delete;

  /////////////////////////////////////////////////////////////////////////////
  // Log entry helpers
  /////////////////////////////////////////////////////////////////////////////
  template <typename ...T>
  struct event
  {
    event(std::string_view format, T &&...args,
      const std::source_location &src = std::source_location::current())
    {
      const std::string fmt = std::vformat(format, std::make_format_args(args...));
      const std::string msg = std::format("[EVENT] ({}) : {}", src.function_name(), fmt);

      while (log::read) { std::this_thread::yield(); }

      log::data << log::Entry{log::Type::Event, std::move(msg)};
    }
  };

  template <typename ...T>
  struct alert
  {
    alert(std::string_view format, T &&...args,
      const std::source_location &src = std::source_location::current())
    {
      const std::string fmt = std::vformat(format, std::make_format_args(args...));
      const std::string msg = std::format("[ALERT] : ({}) {}", src.function_name(), fmt);

      while (log::read) { std::this_thread::yield(); }
    
      log::data << log::Entry{log::Type::Alert, std::move(msg)};
    }
  };

  template <typename ...T>
  struct error
  {
    error(std::string_view format, T &&...args,
      const std::source_location &src = std::source_location::current())
    {
      const std::string fmt = std::vformat(format, std::make_format_args(args...));
      const std::string msg = std::format("[ERROR] : ({}) {}", src.function_name(), fmt);

      while (log::read) { std::this_thread::yield(); }
    
      log::data << log::Entry{log::Type::Error, std::move(msg)};
    }
  };

  template <typename ...T>
  event(std::string_view, T &&...) -> event<T...>;

  template <typename ...T>
  alert(std::string_view, T &&...) -> alert<T...>;

  template <typename ...T>
  error(std::string_view, T &&...) -> error<T...>;

private:
  static inline std::mutex mutex;
  static inline std::atomic<bool> read{};
}; // class log
} // namespace dt