#pragma once

namespace stdsharp::details
{
    template<typename T>
    T remove_ref(T&&);

    template<typename T>
    T remove_ref(T&);

    template<bool>
    using var_template = void;
}

#define cpp_forward(...) static_cast<decltype(__VA_ARGS__)>(__VA_ARGS__)
#define cpp_move(...) \
    static_cast<decltype(stdsharp::details::remove_ref(__VA_ARGS__))&&>(__VA_ARGS__)
#define cpp_is_constexpr(...) requires { typename details::var_template<(__VA_ARGS__, true)>; }