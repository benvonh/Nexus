#pragma once
#include "dt/declspec.hpp"
#include <string>

namespace dt
{
  struct DT_API IPlugin
  {
    virtual ~IPlugin() = default;

    virtual std::string GetName() const = 0;

    virtual std::string GetVersion() const = 0;

    virtual void Draw() const = 0;

    virtual void Work() const = 0;
  };
}

extern "C"
{
  DT_API dt::IPlugin *create_plugin();
  DT_API void destroy_plugin(dt::IPlugin *plugin);
}