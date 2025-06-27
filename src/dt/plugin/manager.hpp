#pragma once
#include "dt/plugin/module.hpp"
#include <vector>

namespace dt
{
  namespace plugin
  {
    class Manager
    {
    public:
      Manager() = default;
      ~Manager() = default;

      Manager(Manager &&) = delete;
      Manager(const Manager &) = delete;
      Manager &operator=(Manager &&) = delete;
      Manager &operator=(const Manager &) = delete;

      void LoadPlugin(const std::string &path);
      void UnloadPlugin(const std::string &name);

    private:
      std::vector<Module> _plugins;
    };
  }
}