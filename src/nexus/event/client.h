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
    // TODO: Implement unregister functionality
    class Client final : Logger<"Client">
    {
        using Callback = std::function<void(const Event &)>;

    public:
        static void Dispatch()
        {
            s_Mutex.lock();

            while (!s_Queue.empty())
            {
                auto fn = std::move(s_Queue.front());
                s_Queue.pop();
                fn();
            }

            s_Mutex.unlock();
        }

        static void Queue(std::function<void()> &&fn)
        {
            s_Mutex.lock();
            s_Queue.emplace(std::move(fn));
            s_Mutex.unlock();
        }

        template <typename T, typename F>
        static void On(F &&fn)
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
                    throw exception("Cast to {} is null", typeid(T).name());
                }
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