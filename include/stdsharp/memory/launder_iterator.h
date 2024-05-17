#pragma once

#include "../iterator/basic_iterator.h"

#include <new>

namespace stdsharp
{
    template<typename T>
    class launder_iterator : public basic_iterator<std::contiguous_iterator_tag>
    {
        T* ptr_;

        [[nodiscard]] constexpr auto& data() noexcept { return ptr_; }

        friend basic_iterator;

    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        constexpr launder_iterator(T* const ptr) noexcept: ptr_(ptr) {}

        launder_iterator() = default;

        [[nodiscard]] constexpr auto data() const noexcept { return std::launder(ptr_); }

        [[nodiscard]] constexpr operator T*() const noexcept { return data(); }

        [[nodiscard]] constexpr decltype(auto) operator[](const difference_type diff) const noexcept
        {
            assert_not_null(ptr_);
            return *std::launder(ptr_ + diff);
        }

        using basic_iterator::operator[];
    };

    template<typename T>
    launder_iterator(T*) -> launder_iterator<T>;
}