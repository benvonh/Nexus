#include "dt/plugin/module.hpp"
#include "dt/util.hpp"

dt::plugin::Module::Module(const std::string &path)
{
  _module = LoadLibraryA(path.c_str());

  if (!_module)
    throw exception("Library module was null! Windows reports, \"{}\"", GetLastError());

  log::debug("Loaded library at '{}'", path);
  auto create = reinterpret_cast<dt::IPlugin *(*)()>(GetProcAddress(_module, "create_plugin"));

  if (!create)
    throw exception("Address for 'create_plugin' was null! Windows reports, \"{}\"", GetLastError());

  _instance = create();

  if (!_instance)
    throw exception("Plugin instance was null!");

  log::debug("Instantiated plugin '{}'", _instance->GetName());
}

dt::plugin::Module::Module(Module &&other) noexcept
    : _module(other._module), _instance(other._instance)
{
  other._module = nullptr;
  other._instance = nullptr;
}

dt::plugin::Module &dt::plugin::Module::operator=(Module &&other) noexcept
{
  if (this != &other)
  {
    if (_module && _instance)
    {
      delete _instance;
      FreeLibrary(_module);
    }
    _module = other._module;
    _instance = other._instance;
    other._module = nullptr;
    other._instance = nullptr;
  }
  return *this;
}

dt::plugin::Module::~Module()
{
  if (!_module || !_instance)
  {
    log::error("This plugin was malformed!");
    return;
  }

  delete _instance;
  FreeLibrary(_module);
}

dt::IPlugin *dt::plugin::Module::operator->() const noexcept
{
  return _instance;
}