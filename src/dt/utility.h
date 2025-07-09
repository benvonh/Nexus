#pragma once

#include <concepts>

namespace dt
{
    template <std::integral T = int, std::integral U>
    [[nodiscard]] constexpr T bit(U x)
    {
        return static_cast<T>(T{1} << x);
    }
}