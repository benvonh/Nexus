#pragma once

#include <memory>
#include <mutex>

namespace dt
{
    ///
    /// @brief Alias to `shared_ptr`
    /// @tparam T Pointer type
    ///
    template <typename T>
    using Ref = std::shared_ptr<T>;

    ///
    /// @brief Make a new `shared_ptr`
    /// @tparam T Pointer type
    /// @tparam ...Args Contsuctor types
    /// @param ...args Constuctor arguments
    /// @return New `shared_ptr`
    ///
    template <typename T, typename... Args>
    [[nodiscard]] inline constexpr Ref<T> NewRef(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    ///
    /// @brief Alias to `unique_ptr`
    /// @tparam T Pointer type
    ///
    template <typename T>
    using Ptr = std::unique_ptr<T>;

    ///
    /// @brief Make a new `unique_ptr`
    /// @tparam T Pointer type
    /// @tparam ...Args Constructor types
    /// @param ...args Constructor arguments
    /// @return New `unique_ptr`
    ///
    template <typename T, typename... Args>
    [[nodiscard]] inline constexpr Ptr<T> NewPtr(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    ///
    /// @brief A simple RAII wrapper for a reference and mutex
    /// @tparam T Reference type
    /// @tparam M Mutex type, default `std::mutex`
    ///
    /// TIP: Use structured binding, e.g.,
    ///      `auto [ref, guard] = Permit<T>(obj, &mutex);`
    ///
    /// WARNING: Never take a reference to a "permit"
    ///          as the reference will be invalidated
    ///
    template <typename T, typename M = std::mutex>
    struct Permit
    {
        Permit(T &r, M *m) : Reference(r), Guard(*m) {}

        T &Reference;

        std::lock_guard<M> Guard;
    };
}