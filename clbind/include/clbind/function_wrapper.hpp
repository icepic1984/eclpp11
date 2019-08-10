#pragma once

#include <ecl/ecl.h>
#include <clbind/function_traits.hpp>
#include <clbind/ecl_utilities.hpp>

namespace clbind
{

namespace detail
{
template <typename T>
struct function_wrapper;

template <typename... Args>
struct function_wrapper<std::tuple<Args...>>
{
    template <typename F>
    static cl_object wrap(F&& f, cl_object frame)
    {
        wrap_helper(
            std::forward<F>(f), std::index_sequence_for<Args...>{}, frame);
    }

private:
    template <typename F, std::size_t... Index>
    static cl_object wrap_helper(
        F&& f, std::index_sequence<Index...>, cl_object frame)
    {
        if constexpr (std::is_same_v<void, function_return_type_t<F>>)
        {

            std::invoke(f, clbind::to_cpp<std::tuple_element_t<Index, Tuple>>(
                               clbind::nth_arg(frame, Index))...);
            return ECL_NIL;
        }
        else
        {
            return clbind::to_ecl<function_return_type_t<F>>(std::invoke(
                f, clbind::to_cpp<std::tuple_element_t<Index, Tuple>>(
                       clbind::nth_arg(frame, Index))...));
        }
    }

    using Tuple = std::tuple<Args...>;
};

} // namespace detail

template <typename F>
cl_object wrap(F&& func, cl_object frame)
{
    detail::function_wrapper<function_args_t<F>>::wrap(
        std::forward<F>(func), frame);
}

} // namespace clbind
