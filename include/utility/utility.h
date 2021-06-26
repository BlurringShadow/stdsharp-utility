﻿// Created by BlurringShadow at 2021-02-27-下午 10:24

#pragma once

#include <gsl/gsl>
#include <utility>
#include <concepts>

namespace blurringshadow::utility
{
    using namespace std::literals;

    namespace details
    {
        template<typename T>
        struct auto_cast
        {
            T t;

            template<typename U>
            [[nodiscard]] constexpr operator U() const&& noexcept(noexcept(static_cast<U>(t)))
            {
                return static_cast<U>(t);
            }

            template<typename U>
            [[nodiscard]] constexpr U operator()() const&& noexcept(noexcept(static_cast<U>(t)))
            {
                return static_cast<U>(t);
            }
        };
    }

    inline constexpr auto auto_cast = []<typename T>(T&& t) // clang-format off
        noexcept(std::is_nothrow_constructible_v<details::auto_cast<T>, T>)
    {
        return details::auto_cast<T>{std::forward<T>(t)};
    }; // clang-format on

    namespace details
    {
        struct to_underlying_fn
        {
            template<typename T>
                requires std::is_enum_v<T>
            [[nodiscard]] constexpr std::underlying_type_t<T> operator()(const T v) const noexcept
            {
                return v;
            }
        };
    }

    inline constexpr details::to_underlying_fn to_underlying;
}