#pragma once
#include "digital_twin/plugin.hpp"
#include <Windows.h>
#include <string>

namespace dt
{
  namespace plugin
  {
    class Module
    {
    public:
      Module(const std::string &path);
      Module(Module &&other) noexcept;
      Module &operator=(Module &&other) noexcept;
      ~Module() noexcept;

      Module(const Module &) = delete;
      Module &operator=(const Module &) = delete;

      dt::IPlugin *operator->() const noexcept;

    private:
      HMODULE _module = nullptr;
      dt::IPlugin *_instance = nullptr;
    };
  }
}