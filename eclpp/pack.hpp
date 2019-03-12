#pragma once
#include <boost/callable_traits/args.hpp>

namespace eclpp
{

struct none_t;

template <typename... TS>
struct pack
{
};

template <typename Pack>
struct pack_size
{
};

template <typename... Ts>
struct pack_size<pack<Ts...>>
{
    static constexpr auto value = sizeof...(Ts);
};

template <typename Pack>
constexpr std::size_t pack_size_v = pack_size<Pack>::value;

template <typename Pack>
struct pack_last
{
    using type = none_t;
};

template <typename T, typename... Ts>
struct pack_last<pack<T, Ts...>> : pack_last<pack<Ts...>>
{
};

template <typename T>
struct pack_last<pack<T>>
{
    using type = T;
};

template <typename Pack>
using pack_last_t = typename pack_last<Pack>::type;

template <typename Fn>
using function_args_t = boost::callable_traits::args_t<Fn, pack>;

} // namespace eclpp
