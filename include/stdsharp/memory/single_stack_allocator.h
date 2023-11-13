#pragma once

#include "single_stack_buffer.h"

namespace stdsharp
{
    template<typename T, std::size_t Size>
    class single_stack_allocator
    {
    public:
        using value_type = T;
        using propagate_on_container_move_assignment = std::true_type;

        static constexpr auto size = Size;

        using resource_type = single_stack_buffer<size * sizeof(T)>;

        constexpr explicit single_stack_allocator(resource_type& src) noexcept: src_(src) {}

        template<typename U>
        struct rebind
        {
            using other = single_stack_allocator<U, Size>;
        };

        template<typename U>
        constexpr single_stack_allocator(const typename rebind<U>::other other) noexcept:
            single_stack_allocator(other.resource())
        {
        }

        [[nodiscard]] constexpr T* allocate(const std::size_t size)
        {
            return pointer_cast<T>(resource().allocate(byte_size(size)));
        }

        [[nodiscard]] constexpr T* try_allocate(const std::size_t size)
        {
            return pointer_cast<T>(resource().try_allocate(byte_size(size)));
        }

        constexpr void deallocate(T* const ptr, const std::size_t size) noexcept
        {
            resource().deallocate(to_void_pointer(ptr), byte_size(size));
        }

        [[nodiscard]] constexpr resource_type& resource() const noexcept { return src_.get(); }

        [[nodiscard]] constexpr bool operator==(const single_stack_allocator other) const noexcept
        {
            return resource() == other.resource();
        }

        [[nodiscard]] constexpr bool contains(const void* const ptr) const noexcept
        {
            return resource().contains(ptr);
        }

    private:
        std::reference_wrapper<resource_type> src_;

        static constexpr auto byte_size(const std::size_t size) { return size * sizeof(T); }
    };

    template<std::size_t Size>
    single_stack_allocator(single_stack_buffer<Size>&) -> single_stack_allocator<byte, Size>;

    template<typename T = byte>
    struct make_single_stack_allocator_fn
    {
        template<std::size_t Size>
            requires((Size % sizeof(T)) == 0)
        [[nodiscard]] constexpr auto operator()(single_stack_buffer<Size>& buffer) const noexcept
        {
            return single_stack_allocator<T, Size / sizeof(T)>{buffer};
        }
    };

    template<typename T = byte>
    inline constexpr make_single_stack_allocator_fn<T> make_single_stack_allocator{};
}