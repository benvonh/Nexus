#pragma once

#include <concepts>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string_view>
#include <type_traits>

namespace Nexus
{
    ///
    /// @brief A dummy mutex satisfying BasicLockable.
    ///
    struct NullMutex
    {
        consteval void lock() noexcept {}
        consteval void unlock() noexcept {}
    };

    ///
    /// @brief Implementation of the BasicLockable concept
    /// <https://en.cppreference.com/w/cpp/named_req.html>.
    ///
    template <class M>
    concept BasicLockable = requires(M m) {
        m.lock();
        m.unlock();
    };

    ///
    /// @brief A meta-programmable string.
    /// @tparam N Length of string
    ///
    template <std::size_t N>
    struct MetaString
    {
        char String[N] = {};

        consteval MetaString() = default;

        consteval MetaString(const char (&str)[N])
        {
            std::copy_n(str, N, this->String);
        }

        consteval auto size() const { return N; }

        consteval operator std::string_view() const
        {
            return std::string_view(this->String, N - 1);
        }

        constexpr char &operator[](std::size_t index)
        {
            return this->String[index];
        }

        constexpr char operator[](std::size_t index) const
        {
            return this->String[index];
        }

        template <std::size_t M>
        consteval auto operator+(const MetaString<M> &other) const
        {
            MetaString<N + M - 1> result;
            std::copy_n(this->String, N - 1, result.String);
            std::copy_n(other.String, M, result.String + N - 1);
            return result;
        }
    };

    ///
    /// @brief Concatenates a `MetaString` with a string literal.
    /// @tparam N Length of the `MetaString`
    /// @tparam M Length of the string literal
    /// @param lhs The `MetaString` to concatenate
    /// @param rhs The string literal to concatenate
    /// @return The concatenated string as a `MetaString`
    ///
    template <std::size_t N, std::size_t M>
    consteval auto operator+(const MetaString<N> &lhs, const char (&rhs)[M])
    {
        return lhs + MetaString(rhs);
    }

    ///
    /// @brief Concatenates a `MetaString` with a string literal.
    /// @tparam N Length of the string literal
    /// @tparam M Length of the `MetaString`
    /// @param lhs The string literal to concatenate
    /// @param rhs The `MetaString` to concatenate
    /// @return The concatenated string as a `MetaString`
    ///
    template <std::size_t N, std::size_t M>
    consteval auto operator+(const char (&lhs)[N], const MetaString<M> &rhs)
    {
        return MetaString(lhs) + rhs;
    }

    ///
    /// @brief A locked resource to a mutable reference.
    /// @tparam T Reference type
    /// @tparam M Mutex type
    ///
    template <typename T, class M>
        requires(BasicLockable<M>)
    struct WriteAccess
    {
        WriteAccess(T *r, M *m) : Ref(*r), Lock(*m) {}

        T *operator->() { return &Ref; }

        T &operator*() { return Ref; }

        T &Ref;

        std::scoped_lock<M> Lock;
    };

    ///
    /// @brief A locked resource to a constant reference.
    /// @tparam T Reference type
    /// @tparam M Mutex type
    ///
    template <typename T, class M>
        requires(BasicLockable<M>)
    struct ReadAccess
    {
        ReadAccess(const T *r, M *m) : Ref(*r), Lock(*m) {}

        const T *operator->() { return &Ref; }

        const T &operator*() { return Ref; }

        const T &Ref;

        std::shared_lock<M> Lock;
    };

    ///
    /// @brief An array that wraps around when the buffer is full.
    /// @tparam T Element type
    /// @tparam N Buffer size
    /// @tparam M Mutex type (default: no mutex)
    ///
    template <typename T, std::size_t N, class M = NullMutex>
        requires(BasicLockable<M>)
    class CircularBuffer
    {
    public:
        class Iterator
        {
        public:
            using pointer = T *;
            using reference = T &;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            Iterator(pointer p, std::size_t i, M *m) noexcept(std::is_same_v<M, NullMutex>)
                : m_Guard(*m), m_Ptr(p + i), m_Countdown(N - i)
            {
            }

            Iterator &operator++() noexcept
            {
                if (--m_Countdown == 0)
                {
                    m_Ptr -= N;
                }
                m_Ptr++;
                return *this;
            }

            bool operator!=(const T *p) const noexcept
            {
                return m_Countdown > 0 || m_Ptr != p;
            }

            const T *operator->() const noexcept { return m_Ptr; }

            const T &operator*() const noexcept { return *m_Ptr; }

        private:
            std::lock_guard<M> m_Guard;

            pointer m_Ptr;

            long m_Countdown;
        };

        void push(const T &elem)
        {
            std::lock_guard guard(m_Mutex);
            m_Buffer[m_Index] = elem;
            ++(*this);
        }

        void push(T &&elem)
        {
            std::lock_guard guard(m_Mutex);
            m_Buffer[m_Index] = std::move(elem);
            ++(*this);
        }

        template <typename... Args>
        void emplace(Args &&...args)
        {
            std::lock_guard guard(m_Mutex);
            m_Buffer[m_Index].~T();
            new (&m_Buffer[m_Index]) T(std::forward<Args>(args)...);
            ++(*this);
        }

        [[nodiscard]]
        auto begin() noexcept(std::is_same_v<M, NullMutex>)
        {
            return Iterator(&m_Buffer[0], m_Index, &m_Mutex);
        }

        [[nodiscard]]
        auto end() const noexcept
        {
            return &m_Buffer[m_Index];
        }

        [[nodiscard]]
        auto page()
        {
            return WriteAccess(&m_Buffer[m_Index], &m_Mutex);
        }

        [[nodiscard]]
        consteval auto size() const noexcept
        {
            return N;
        }

        void next() noexcept { ++(*this); }

        void lock() { m_Mutex.lock(); }

        void unlock() { m_Mutex.unlock(); }

        void operator++() noexcept { m_Index = ++m_Index % N; }

    private:
        T m_Buffer[N];

        M m_Mutex;

        std::size_t m_Index = 0;
    };
}