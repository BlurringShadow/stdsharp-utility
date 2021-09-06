#include "traits/value_sequence_test.h"
#include "utility/traits/value_sequence.h"

namespace std_sharp::test::utility::traits
{
    namespace
    {
        template<auto I, size_t Expect>
        using get_test_params = static_params<I, Expect>;

        template<template<auto...> typename T>
        struct apply_t_test_params
        {
        };

        template<typename, size_t...>
        struct indexed_by_seq_t_test_params
        {
        };

        template<typename Seq, typename Expect>
        using indexed_t_test_params = std::tuple<Seq, Expect>;

        template<typename Seq, typename Expect, typename FrontExpect>
        using append_by_seq_t_test_params = std::tuple<Seq, Expect, FrontExpect>;

        template<typename Seq, typename Expect>
        using remove_t_test_params = std::tuple<Seq, Expect>;

        template<size_t, typename, typename>
        struct insert_by_seq_t_test_params
        {
        };

        template<typename, auto...>
        struct unique_seq_t_test_params
        {
        };
    }

    template<auto... V>
    using value_sequence = std_sharp::utility::traits::value_sequence<V...>;

    template<auto... V>
    using regular_value_sequence = std_sharp::utility::traits::regular_value_sequence<V...>;

    boost::ut::suite& value_sequence_test()
    {
        static boost::ut::suite suite = []
        {
            using namespace std;
            using namespace boost::ut;
            using namespace bdd;
            using namespace std_sharp::utility;

            using test_seq = value_sequence<0, 1, size_t{7}, 1, to_array("my literal")>;

            println(fmt::format("test_seq type: {}", reflection::type_name<test_seq>()));

            feature("construct") = []
            {
                static_expect<default_initializable<value_sequence<>>>();
                static_expect<default_initializable<test_seq>>();
            };

            feature("get") = []<auto I, size_t Expect>(const get_test_params<I, Expect>)
            {
                given("given index") = []
                {
                    print(fmt::format("index: {}", I));

                    then("indexed value should be expected value") = []
                    {
                        print(fmt::format("expected value: {}", Expect));
                        static_expect<test_seq::get<I>() == Expect>();
                    };
                }; // clang-format off
            } | tuple<
                get_test_params<0, 0>,
                get_test_params<1, 1>,
                get_test_params<2, 7>,
                get_test_params<3, 1>
            >{}; // clang-format on

            feature("invoke") = []<typename T>(const T)
            {
                given("given function") = []
                {
                    print(fmt::format("function type: {}", reflection::type_name<T>()));

                    then("sequence invoke should be invocable") = [] //
                    {
                        static_expect<invocable<decltype(test_seq::invoke), T>>(); //
                    };
                };
            } | tuple<identity>{};

            feature("find") = []<auto V, auto Expect>(const static_params<V, Expect>)
            {
                given("given value") = []
                {
                    print(fmt::format("value: {}", V));

                    then("found index should be expected") = []
                    {
                        constexpr auto i = test_seq::find(V);
                        print(fmt::format("expected: {}", Expect));
                        static_expect<i == Expect>() << fmt::format("actual value: {}", i);
                    };
                }; // clang-format off
            } | tuple<
                static_params<0, 0>,
                static_params<1, 1>,
                static_params<'!', test_seq::size>
            >{}; // clang-format on

            feature("count") = []<auto V, auto Expect>(const static_params<V, Expect>)
            {
                given("given value") = []
                {
                    print(fmt::format("value: {}", V));

                    then("count should be expected") = []
                    {
                        constexpr auto count = test_seq::count(V);
                        print(fmt::format("expected: {}", Expect));
                        static_expect<count == Expect>() << fmt::format("actual count: {}", count);
                    };
                }; // clang-format off
            } | tuple<static_params<0, 1>, static_params<1, 2>, static_params<'?', 0>>{};

            // clang-format on
            feature("apply_t") = []<template<auto...> typename T>(const apply_t_test_params<T>)
            {
                given("given template") = []
                {
                    print(fmt::format("template: {}", reflection::type_name<T<>>()));

                    then("type that is applied sequence values should be constructible") = []
                    {
                        static_expect<_b(default_initializable<test_seq::apply_t<T>>)>(); //
                    };
                }; // clang-format off
            } | tuple<apply_t_test_params<static_params>>{}; // clang-format on

            // TODO here the braces to avoid strange MSVC compile error
            // TODO clang lambda NTTP
            {
                constexpr auto lam_1 = [](const int v) mutable { return v + 1; };
                constexpr auto lam_2 = [](const int v) { return v + 42; };
                constexpr auto lam_3 = [](const size_t v) { return v + 42; };
                constexpr auto lam_4 = [](const int v) { return v + 6; };
                constexpr auto lam_5 = []<auto Size>(const array<char, Size>& str)
                {
                    return str[0];
                };

                feature("transform_t") = []<auto... Functor>(const static_params<Functor...>)
                {
                    static_expect<
                        default_initializable<test_seq::template transform_t<Functor...>>>();
                    // clang-format off
                } | tuple<
                    static_params<identity_v>,
                    static_params<lam_1, lam_2, lam_3, lam_4, lam_5>
                >{}; // clang-format on
            }

            // clang-format off
            feature("indexed_by_seq_t") = []<typename T, typename Expect>(
                const indexed_t_test_params<T, Expect>
            ) // clang-format on
            {
                given("given indices sequence") = []
                {
                    print(fmt::format("indices type: {}", reflection::type_name<T>()));

                    then("use indices type as indexed_by_seq_t template arg, "
                         "type should be expected") = []
                    {
                        print(fmt::format("expected type: {}", reflection::type_name<Expect>()));
                        using actual_t = test_seq::indexed_by_seq_t<T>;
                        static_expect<_b(same_as<actual_t, Expect>)>() << //
                            fmt::format("actual type: {}", reflection::type_name<actual_t>());
                    };
                }; // clang-format off
            } | tuple<
                indexed_t_test_params<
                    regular_value_sequence<1, 2>,
                    regular_value_sequence<1, size_t{7}>
                >,
                indexed_t_test_params<
                    regular_value_sequence<2, 4>,
                    regular_value_sequence<size_t{7}, to_array("my literal")>
                >
            >{}; // clang-format on

            // clang-format off
            feature("append_by_seq_t") = []<typename Seq, typename Expect, typename FrontExpect>(
                const append_by_seq_t_test_params<Seq, Expect, FrontExpect>
            ) // clang-format on
            {
                given("given sequence") = []
                {
                    print(fmt::format("sequence type: {}", reflection::type_name<Seq>()));

                    then("use seq type as append_by_seq_t template arg, type should be expected") =
                        []
                    {
                        print(fmt::format("expected type: {}", reflection::type_name<Expect>()));

                        using actual_t = test_seq::append_by_seq_t<Seq>;
                        static_expect<same_as<actual_t, Expect>>() << //
                            fmt::format("actual type: {}", reflection::type_name<actual_t>());
                    };

                    then("use seq type as append_front_by_seq_t template arg, "
                         "type should be expected") = []
                    {
                        print(
                            fmt::format("expected type: {}", reflection::type_name<FrontExpect>()));
                        using actual_t = test_seq::append_front_by_seq_t<Seq>;
                        static_expect<same_as<actual_t, FrontExpect>>() << //
                            fmt::format("actual type: {}", reflection::type_name<actual_t>());
                    };
                }; // clang-format off
            } | tuple<
                append_by_seq_t_test_params<
                    regular_value_sequence<1, 2>,
                    regular_value_sequence<
                        0, 1, size_t{7}, 1, to_array("my literal"), 1, 2
                    >,
                    regular_value_sequence<
                        1, 2, 0, 1, size_t{7}, 1, to_array("my literal")
                    >
                >,
                append_by_seq_t_test_params<
                    regular_value_sequence<2, 4>,
                    regular_value_sequence<
                        0, 1, size_t{7}, 1, to_array("my literal"), 2, 4
                    >,
                    regular_value_sequence<
                        2, 4, 0, 1, size_t{7}, 1, to_array("my literal")
                    >
                >
            >{}; // clang-format on

            // clang-format off
            feature("insert_by_seq_t") = []<size_t Index, typename Seq, typename Expect>(
                const insert_by_seq_t_test_params<Index, Seq, Expect>
            ) // clang-format on
            {
                given("given sequence") = []
                {
                    print(fmt::format("sequence type: {}", reflection::type_name<Seq>()));

                    then("use seq type as insert_by_seq_t template arg, type should be expected") =
                        []
                    {
                        print(fmt::format("expected type: {}", reflection::type_name<Expect>()));
                        using actual_t = test_seq::insert_by_seq_t<Index, Seq>;
                        static_expect<same_as<actual_t, Expect>>() << //
                            fmt::format("actual type: {}", reflection::type_name<actual_t>());
                    };
                }; // clang-format off
            } | tuple<
                insert_by_seq_t_test_params<
                    3,
                    regular_value_sequence<1, 2>,
                    regular_value_sequence<
                        0, 1, size_t{7}, 1, 2, 1, to_array("my literal")
                    >
                >,
                insert_by_seq_t_test_params<
                    5,
                    regular_value_sequence<2, 4>,
                    regular_value_sequence<
                        0, 1, size_t{7}, 1, to_array("my literal"), 2, 4
                    >
                >
            >{}; // clang-format on

            // clang-format off
            feature("remove_at_by_seq_t") = []<typename T, typename Expect>(
                const indexed_t_test_params<T, Expect>
            ) // clang-format on
            {
                given("given indices sequence") = []
                {
                    print(fmt::format("indices type: {}", reflection::type_name<T>()));

                    then("use indices type as remove_at_by_seq_t template arg, "
                         "type should be expected") = []
                    {
                        print(fmt::format("expected type: {}", reflection::type_name<Expect>()));
                        using actual_t = test_seq::remove_at_by_seq_t<T>;
                        static_expect<_b(same_as<actual_t, Expect>)>() << //
                            fmt::format("actual type: {}", reflection::type_name<actual_t>());
                    };
                }; // clang-format off
            } | tuple<
                indexed_t_test_params<
                    regular_value_sequence<1, 2>,
                    regular_value_sequence<0, 1, to_array("my literal")>
                >,
                indexed_t_test_params<
                    regular_value_sequence<2, 4>,
                    regular_value_sequence<0, 1, 1>
                >
            >{}; // clang-format on

            // clang-format off
            feature("unique_value_sequence_t") = []<typename Expect, auto... Values>(
                const unique_seq_t_test_params<Expect, Values...>
            ) // clang-format on
            {
                given("given values") = []
                {
                    constexpr auto pack_size = sizeof...(Values);
                    string format_str = "values: ";

                    if constexpr(pack_size > 0)
                    {
                        constexpr auto single_format_str = to_array("{}, ");
                        format_str.reserve(
                            format_str.size() + pack_size * single_format_str.size() //
                        );
                        for(size_t i = 0; i < pack_size - 1; ++i)
                            format_str += single_format_str.data();
                        format_str += "{}";
                    }

                    print(fmt::format(format_str, Values...));

                    then("use seq as unique_value_sequence_t template arg, "
                         "type should be expected") = []
                    {
                        print(fmt::format("expected type: {}", reflection::type_name<Expect>()));
                        using actual_t =
                            std_sharp::utility::traits::unique_value_sequence_t<Values...>;
                        static_expect<_b(same_as<actual_t, Expect>)>() << //
                            fmt::format("actual type: {}", reflection::type_name<actual_t>());
                    };
                }; // clang-format off
            } | tuple<
                unique_seq_t_test_params<regular_value_sequence<>>,
                unique_seq_t_test_params<
                    regular_value_sequence<0, 10, 1, 5>,
                    0, 10, 1, 5, 10, 1
                >
            >{}; // clang-format on
        };

        return suite;
    }
}
