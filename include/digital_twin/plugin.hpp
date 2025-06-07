#pragma once
#include <string>

#ifdef PLUGIN_BUILD
  #define PLUGIN_API __declspec(dllexport)
#else
  #define PLUGIN_API __declspec(dllimport)
#endif

namespace dt
{
  struct PLUGIN_API IPlugin
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
  PLUGIN_API dt::IPlugin *create_plugin();
  PLUGIN_API void destroy_plugin(dt::IPlugin *plugin);
}