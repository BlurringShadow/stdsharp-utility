#pragma once

#include "../cassert/cassert.h"
#include "../cstdint/cstdint.h"
#include "../iterator/basic_iterator.h"

#include <bitset>
#include <ranges>

#include "../compilation_config_in.h"

namespace stdsharp::details
{
    template<typename Bitset>
    class bitset_iterator : public basic_iterator<decltype(Bitset{}[0]), ssize_t>
    {
    public:
        using bitset = Bitset;

    protected:
        bitset* set_{};
        std::size_t i_{};

    public:
        bitset_iterator() = default;

        constexpr bitset_iterator(bitset& set, const std::size_t index): set_(&set), i_(index)
        {
        }

        [[nodiscard]] constexpr decltype(auto) operator*() const { return (*set_)[i_]; }

        constexpr auto& operator++() noexcept
        {
            ++i_;
            return *this;
        }

        constexpr auto& operator--() noexcept
        {
            --i_;
            return *this;
        }

        constexpr auto& operator+=(const ssize_t n) noexcept
        {
            i_ += n;
            return *this;
        }

        [[nodiscard]] constexpr auto operator-(const bitset_iterator& other) const noexcept
        {
            assert_equal(this->set_, other.set_);
            return static_cast<ssize_t>(i_) - static_cast<ssize_t>(other.i_);
        }

        [[nodiscard]] constexpr decltype(auto) operator[](const ssize_t index) const
        {
            return set_[i_ + index];
        }

        [[nodiscard]] constexpr bool operator==(const bitset_iterator& other) const noexcept
        {
            assert_equal(set_, other.set_);
            return i_ == other.i_;
        }

        [[nodiscard]] constexpr auto operator<=>(const bitset_iterator& other) const noexcept
        {
            assert_equal(set_, other.set_);
            return i_ <=> other.i_;
        }
    };
}

namespace stdsharp
{
    template<std::size_t N>
    struct bitset_iterator : details::bitset_iterator<std::bitset<N>>
    {
    private:
        using m_base = details::bitset_iterator<std::bitset<N>>;

    public:
        using m_base::m_base;
    };

    template<std::size_t N>
    bitset_iterator(std::bitset<N>&, auto) -> bitset_iterator<N>;

    template<std::size_t N>
    struct bitset_const_iterator : details::bitset_iterator<const std::bitset<N>>
    {
    private:
        using m_base = details::bitset_iterator<const std::bitset<N>>;

    public:
        using m_base::m_base;
    };

    template<std::size_t N>
    bitset_const_iterator(const std::bitset<N>&, auto) -> bitset_const_iterator<N>;

    inline constexpr struct bitset_crng_fn
    {
        template<std::size_t N>
        [[nodiscard]] constexpr auto operator()(const std::bitset<N>& set) const
        {
            return std::ranges::
                subrange{bitset_const_iterator{set, 0}, bitset_const_iterator{set, N}};
        }
    } bitset_crng{};

    inline constexpr struct bitset_rng_fn : bitset_crng_fn
    {
        using bitset_crng_fn::operator();

        template<std::size_t N>
        [[nodiscard]] constexpr auto operator()(std::bitset<N>& set) const
        {
            return std::ranges::subrange{bitset_iterator{set, 0}, bitset_iterator{set, N}};
        }
    } bitset_rng{};
}

#include "../compilation_config_out.h"
