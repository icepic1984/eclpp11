#include <ecl/ecl.h>
#include <cstdint>
#include <iostream>

namespace eclpp
{
template <typename T, typename Enable = void>
struct convert;

template <>
struct convert<int>
{
    static int to_cpp(cl_object v)
    {
        return fix(v);
    }

    static cl_object to_ecl(int v)
    {
        return ecl_make_integer(v);
    }
};

template <>
struct convert<std::uint8_t>
{
    static std::uint8_t to_cpp(cl_object v)
    {
        return ecl_to_uint8_t(v);
    }

    static cl_object to_ecl(std::uint8_t v)
    {
        return ecl_make_uint8_t(v);
    }
};

template <>
struct convert<std::int8_t>
{
    static std::int8_t to_cpp(cl_object v)
    {
        return ecl_to_int8_t(v);
    }

    static cl_object to_ecl(std::int8_t v)
    {
        return ecl_make_int8_t(v);
    }
};

template <>
struct convert<double>
{
    static double to_cpp(cl_object v)
    {
        std::cout << "df" << std::endl;
        return ecl_to_double(v);
    }

    static cl_object to_ecl(double v)
    {
        std::cout << "ddfdff" << std::endl;
        return ecl_make_double_float(v);
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
};
} // namespace eclpp

// template <typename T>
// auto to_scm(T&& v) -> ECLPP_DECLTYPE_RETURN(
//     convert<std::decay_t<T>>::to_scm(std::forward<T>(v)));

// template <typename T>
// auto to_cpp(SCM v)
//     -> ECLPP_DECLTYPE_RETURN(convert<std::decay_t<T>>::to_cpp(v));
