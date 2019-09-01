#pragma once

#include <type_traits>
#include <utility>
#include <ecl/ecl.h>

namespace clbind
{

namespace detail
{

template <typename T, typename Enable = void>
struct convert;

template <>
struct convert<int>
{
    static int to_cpp(cl_object v)
    {
        return ecl_to_int(v);
    }

    static cl_object to_ecl(int v)
    {
        return ecl_make_int(v);
    }

    static std::string type_string()
    {
        return " :int";
    }
};

template <>
struct convert<float>
{
    static float to_cpp(cl_object v)
    {
        return ecl_to_float(v);
    }

    static cl_object to_ecl(float v)
    {
        return ecl_make_single_float(v);
    }

    static std::string type_string()
    {
        return " :float";
    }
};

template <>
struct convert<double>
{
    static double to_cpp(cl_object v)
    {
        return ecl_to_double(v);
    }

    static cl_object to_ecl(double v)
    {
        return ecl_make_double_float(v);
    }

    static std::string type_string()
    {
        return " :double";
    }
};

} // namespace detail

template <typename T>
decltype(auto) to_cpp(cl_object v)
{
    return clbind::detail::convert<std::decay_t<T>>::to_cpp(v);
}

template <typename T>
decltype(auto) to_ecl(T&& v)
{
    return clbind::detail::convert<std::decay_t<decltype(v)>>::to_ecl(
        std::forward<T>(v));
}

template <typename T>
decltype(auto) to_type_string()
{
    if constexpr (std::is_same_v<void, T>)
    {
        return ":void";
    }
    else
    {
        return clbind::detail::convert<std::decay_t<T>>::type_string();
    }
}

} // namespace clbind
