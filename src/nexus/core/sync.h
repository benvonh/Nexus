#pragma once

#include <shared_mutex>

namespace Nexus
{
    class World;
    class Entity;

    class Sync
    {
        friend class World;
        friend class Entity;

        static void Lock() { Mutex.lock(); }
        static void Unlock() { Mutex.unlock(); }

        static inline std::shared_mutex Mutex;
    };
}