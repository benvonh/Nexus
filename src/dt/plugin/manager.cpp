#pragma once
#include "dt/plugin/manager.hpp"
#include "dt/util.hpp"

void dt::plugin::Manager::LoadPlugin(const std::string &path)
{
  _plugins.emplace_back(path);
}

void dt::plugin::Manager::UnloadPlugin(const std::string &name)
{
  auto it = std::find_if(_plugins.begin(), _plugins.end(),
                         [&name](const Module &module)
                         { return module->GetName() == name; });

  if (it != _plugins.end())
  {
    _plugins.erase(it);
    log::event("Plugin '{}' was removed", name);
  }
  else
  {
    log::alert("Could not find plugin with the name '{}'", name);
  }
}