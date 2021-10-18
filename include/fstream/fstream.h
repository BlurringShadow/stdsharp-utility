#pragma once
#include <fstream>
#include <filesystem>

#include "containers/actions.h"
#include "functional/invocable_obj.h"

namespace stdsharp::fstream
{
    namespace details
    {
        template<typename T>
        struct get_from_stream_fn
        {
            template<typename InputStream, typename... Args>
                requires ::std::derived_from<::std::decay_t<InputStream>, ::std::istream> && //
                    ::std::constructible_from<T, Args...>
            [[nodiscard]] constexpr auto operator()(InputStream&& is, Args&&... args) const
            {
                T t{::std::forward<Args>(args)...};
                ::std::forward<InputStream>(is) >> t;
                return t;
            }
        };
    }

    template<typename T>
    inline constexpr ::stdsharp::fstream::details::get_from_stream_fn<T> get_from_stream{};

    namespace details
    {
        template<typename T>
            requires ::std::invocable<
                ::stdsharp::fstream::details::get_from_stream_fn<T>,
                ::std::ifstream // clang-format off
            > // clang-format on
        struct container_read_all_fn
        {
            template<typename Container = ::std::vector<T>>
                requires ::std::invocable<
                    decltype(::stdsharp::containers::actions::emplace_back),
                    Container&,
                    T // clang-format off
                > // clang-format on
            [[nodiscard]] constexpr auto&
                operator()(Container& container, const ::std::filesystem::path& path) const
            {
                ::std::ifstream fs(path);
                while(fs)
                    ::stdsharp::containers::actions:: //
                        emplace_back(container, ::stdsharp::fstream::get_from_stream<T>(fs));

                return container;
            }
        };
    }

    template<typename T>
    inline constexpr ::stdsharp::fstream::details::container_read_all_fn<T> container_read_all{};

    namespace details
    {
        template<typename T, ::std::constructible_from Container>
            requires ::std::invocable<
                ::stdsharp::fstream::details::container_read_all_fn<T>,
                ::std::add_lvalue_referene_t<Container>,
                ::std::filesystem::path // clang-format off
            > // clang-format on
        struct read_all_fn
        {
            [[nodiscard]] constexpr auto operator()(const ::std::filesystem::path& path) const
            {
                Container container{};
                return ::stdsharp::fstream::container_read_all<T>(container, path);
            }
        };
    }

    template<typename T, typename Container = ::std::vector<T>>
    inline constexpr ::stdsharp::fstream::details::read_all_fn<T, Container> read_all{};
}