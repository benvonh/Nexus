#pragma once

#include <concepts>

namespace Nexus
{
    namespace nxs
    {
        template <std::integral T = int, std::integral U>
        [[nodiscard]] constexpr T bit(U x)
        {
            return static_cast<T>(T{1} << x);
        }
    }
}