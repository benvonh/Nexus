#pragma once

#include "event.h"

#include "dt/types.h"
#include "dt/logging.h"
#include "dt/exception.h"

#include <vector>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace dt
{
    // TODO: Implement unregiester functionality
    struct Client
    {
        using Callback = std::function<void(const Event &)>;
        using Registry = std::unordered_map<std::type_index, std::vector<Callback>>;

    public:
        void SetRegistryTo(Ref<Registry> &registry)
        {
            DT_THROW_IF(registry != nullptr, std::logic_error);
            _Registry = registry;
        }

    protected:
        template <typename T, typename F>
        void Register(F &&fn)
        {
            const auto index = std::type_index(typeid(T));

            auto wrapper = [fn = std::forward<F>(fn)](const Event &e)
            {
                if (const T *event = dynamic_cast<const T *>(&e))
                {
                    fn(*event);
                }
                else
                {
                    throw exception("Cast to {} was null", typeid(T).name());
                }
            };

            (*_Registry)[index].push_back(wrapper);
        }

        void Notify(const Event &event)
        {
            const auto index = std::type_index(typeid(event));

            const auto handlers = _Registry->find(index);

            if (handlers != _Registry->end())
            {
                for (const auto &fn : handlers->second)
                    fn(event);
            }
            else
            {
                log::alert("Notified  {} but no one cares", typeid(event).name());
            }
        }

    private:
        Ref<Registry> _Registry;
    };
}