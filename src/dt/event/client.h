#pragma once

#include "event.h"

#include "dt/exception.h"
#include "dt/logging.h"
#include "dt/types.h"

#include <functional>
#include <mutex>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace dt
{
    // TODO: Implement unregister functionality
    struct Client
    {
        using Callback = std::function<void(const Event &)>;

    public:
        static void Dispatch()
        {
            S__Mutex.lock();

            while (!S__Queue.empty())
            {
                auto fn = std::move(S__Queue.front());
                S__Queue.pop();
                fn();
            }

            S__Mutex.unlock();
        }

    protected:
        static void Queue(std::function<void()> &&fn)
        {
            S__Mutex.lock();
            S__Queue.emplace(std::move(fn));
            S__Mutex.unlock();
        }

        template <typename T, typename F>
        static void Register(F &&fn)
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
                    throw exception("Cast to {} gave nullptr", typeid(T).name());
                }
            };

            S__Registry[index].push_back(wrapper);
        }

        static void Notify(const Event &event)
        {
            const auto index = std::type_index(typeid(event));

            const auto handlers = S__Registry.find(index);

            if (handlers != S__Registry.end())
            {
                for (const auto &fn : handlers->second)
                    fn(event);
            }
            else
            {
                log::alert("Notified about {} but nobody cares", typeid(event).name());
            }
        }

    private:
        static inline std::mutex S__Mutex;
        static inline std::queue<std::function<void()>> S__Queue;
        static inline std::unordered_map<std::type_index, std::vector<Callback>> S__Registry;
    };
}