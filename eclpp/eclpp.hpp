#include <ecl/ecl.h>
#include <cstdint>
#include <iostream>

namespace eclpp
{
template <typename T, typename Enable = void>
struct convert;

#define ECL_DECLTYPE_RETURN(...)                                               \
    decltype(__VA_ARGS__)                                                      \
    {                                                                          \
        return __VA_ARGS__;                                                    \
    }

double ecl_to_double_float(cl_object v)
{
    return ecl_double_float(v);
}

double ecl_to_single_float(cl_object v)
{
    return ecl_single_float(v);
}

#define ECL_DECLARE_NUMERIC_TYPE(cpp_name__, ecl_name__)                       \
    template <>                                                                \
    struct convert<cpp_name__>                                                 \
    {                                                                          \
        static cpp_name__ to_cpp(cl_object v)                                  \
        {                                                                      \
            return ecl_to_##ecl_name__(v);                                     \
        }                                                                      \
        static cl_object to_ecl(cpp_name__ v)                                  \
        {                                                                      \
            return ecl_make_##ecl_name__(v);                                   \
        }                                                                      \
    };

ECL_DECLARE_NUMERIC_TYPE(std::int8_t, int8_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint8_t, uint8_t);
ECL_DECLARE_NUMERIC_TYPE(std::int16_t, int16_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint16_t, uint16_t);
ECL_DECLARE_NUMERIC_TYPE(std::int32_t, int32_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint32_t, uint32_t);
ECL_DECLARE_NUMERIC_TYPE(std::int64_t, int64_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint64_t, uint64_t);
ECL_DECLARE_NUMERIC_TYPE(float, single_float);
ECL_DECLARE_NUMERIC_TYPE(double, double_float);

template <typename T>
auto to_ecl(T&& v) -> ECL_DECLTYPE_RETURN(
    convert<std::decay_t<T>>::to_ecl(std::forward<T>(v)));

template <typename T>
auto to_cpp(cl_object v)
    -> ECL_DECLTYPE_RETURN(convert<std::decay_t<T>>::to_cpp(v));

} // namespace eclpp
