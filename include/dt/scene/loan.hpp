#pragma once
#include <mutex>

namespace dt
{
  namespace scene
  {
    template <typename T>
    struct Loan final
    {
      Loan(std::mutex &m, T &x) : key(m), item(x) {}
      std::unique_lock<std::mutex> key;
      T &item;
    };
  }
}