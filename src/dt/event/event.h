#pragma once

#include "dt/logging.h"
#include "dt/exception.h"
#include "dt/event/event.h"

#include <vector>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace dt
{
    struct Event
    {
        virtual ~Event() noexcept = default;
    };

    // TODO: Implement unregiester functionality
    struct EventBus
    {
        using Callback = std::function<void(const Event &)>;

    public:
        template <typename T, typename F>
        void Register(F &&fn)
        {
            const auto index = std::type_index(typeid(T));

            auto wrapper = [fn = std::forward<F>(fn)](const Event &e)
            {
                if (const T *event = dynamic_cast<const T *>(&e))
                    fn(*event);
                else
                    throw exception("Cast to {} gave null pointer", typeid(T).name());
            };

            _Lanes[index].push_back(wrapper);
        }

        void Signal(const Event &event)
        {
            const auto index = std::type_index(typeid(event));

            const auto lane = _Lanes.find(index);

            if (lane != _Lanes.end())
            {
                for (const auto &fn : lane->second)
                {
                    fn(event);
                }
            }
            else
            {
                log::alert("Signal emitted for {} but no one cares",
                           typeid(event).name());
            }
        }

    private:
        std::unordered_map<std::type_index, std::vector<Callback>> _Lanes;
    };
}