#pragma once

#include "event.h"

#include "dt/exception.h"
#include "dt/logging.h"

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
        static void dispatch()
        {
            S_Mutex.lock();

            while (!S_Queue.empty())
            {
                auto fn = std::move(S_Queue.front());
                S_Queue.pop();
                fn();
            }

            S_Mutex.unlock();
        }

    protected:
        static void queue(std::function<void()> &&fn)
        {
            S_Mutex.lock();
            S_Queue.emplace(std::move(fn));
            S_Mutex.unlock();
        }

        template <typename T, typename F>
        static void on(F &&fn)
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

            S_Registry[index].push_back(wrapper);
        }

        template <typename T, typename... Args>
        static void send(Args &&...args)
        {
            const auto index = std::type_index(typeid(T));

            const auto handlers = S_Registry.find(index);

            if (handlers != S_Registry.end())
            {
                T event(std::forward<Args>(args)...);
                for (const auto &fn : handlers->second)
                    fn(event);
            }
            else
            {
                log::alert("Sent {} but no one cares", typeid(T).name());
            }
        }

    private:
        static inline std::mutex S_Mutex;
        static inline std::queue<std::function<void()>> S_Queue;
        static inline std::unordered_map<std::type_index, std::vector<Callback>> S_Registry;
    };
}