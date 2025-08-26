#pragma once

#include "nexus/event/event.h"
#include "nexus/exception.h"
#include "nexus/logging.h"

#include <functional>
#include <mutex>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Nexus
{
    class EventClient final
    {
        using Callback = std::function<void(const Event &)>;

    public:
        static void Dispatch()
        {
            std::lock_guard guard(s_Mutex);
            while (!s_Queue.empty())
            {
                auto fn = std::move(s_Queue.front());
                s_Queue.pop();
                fn();
            }
        }

        static void Queue(std::function<void()> &&fn)
        {
            std::lock_guard guard(s_Mutex);
            s_Queue.emplace(std::move(fn));
        }

        template <typename T, typename F>
        static void On(F &&fn)
        {
            const auto index = std::type_index(typeid(T));

            auto wrapper = [fn = std::forward<F>(fn)](const Event &e)
            {
                static_assert(std::is_base_of_v<Event, T>,
                              "Must be an event type");
                static_assert(std::is_invocable_r_v<void, F, const T &>,
                              "Must invoke with event type");
                fn(static_cast<const T &>(e));
            };

            s_Registry[index].push_back(wrapper);
        }

        template <typename T, typename... Args>
        static void Send(Args &&...args)
        {
            const auto index = std::type_index(typeid(T));

            const auto handlers = s_Registry.find(index);

            if (handlers != s_Registry.end())
            {
                T event(std::forward<Args>(args)...);

                for (const auto &fn : handlers->second)
                    fn(event);
            }
            else
            {
                LOG_ALERT("Sent {} but no one cares", typeid(T).name());
            }
        }

    private:
        static inline std::mutex s_Mutex;
        static inline std::queue<std::function<void()>> s_Queue;
        static inline std::unordered_map<std::type_index, std::vector<Callback>> s_Registry;
    };
}