#pragma once

#include <tuple>
#include <memory>
#include <functional>
#include <utility>
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
        return wrap_helper(
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

template <typename R, typename... Args, std::size_t... Index>
cl_object wrapper(const std::function<R(Args...)>& f, cl_object frame,
    std::index_sequence<Index...>)
{
    try
    {
        if constexpr (std::is_same_v<void, R>)
        {
            std::invoke(
                f, clbind::to_cpp<
                       std::tuple_element_t<Index, std::tuple<Args...>>>(
                       clbind::nth_arg(frame, Index))...);
            return ECL_NIL;
        }
        else
        {
            return clbind::to_ecl<R>(std::invoke(
                f, clbind::to_cpp<
                       std::tuple_element_t<Index, std::tuple<Args...>>>(
                       clbind::nth_arg(frame, Index))...));
        }
    }
    catch (const std::exception& e)
    {
        cl_error(1, ecl_make_constant_base_string(e.what(), -1));
        return ECL_NIL;
    }
}

template <typename R, typename... Args, std::size_t... Index>
std::string arguments_to_string(
    const std::function<R(Args...)>& f, std::index_sequence<Index...>)
{
    return (std::string("( ") + ...
               + clbind::to_type_string<
                   std::tuple_element_t<Index, std::tuple<Args...>>>())
           + std::string(" )");
}

} // namespace detail

class function_wrapper_base
{
public:
    virtual void* pointer() = 0;

    virtual ~function_wrapper_base() = default;

    function_wrapper_base() = default;

    function_wrapper_base(const function_wrapper_base&) = delete;

    function_wrapper_base& operator=(const function_wrapper_base&) = delete;
};

template <typename R, typename... Args>
class function_wrapper : public function_wrapper_base
{
public:
    using functor_t = std::function<R(Args...)>;

    explicit function_wrapper(functor_t&& f)
        : m_function(std::move(f))
    {
    }

    void* pointer() final
    {
        return reinterpret_cast<void*>(&m_function);
    }

private:
    functor_t m_function;
};

template <typename F>
cl_object wrap(F&& func, cl_object frame)
{
    return detail::function_wrapper<function_args_t<F>>::wrap(
        std::forward<F>(func), frame);
}

template <typename R, typename... Args>
cl_object wrap2(const std::function<R(Args...)>& f, cl_object frame)
{

    return detail::wrapper(f, frame, std::index_sequence_for<Args...>{});
}

template <typename R, typename... Args>
std::unique_ptr<function_wrapper_base> make_function_wrapper(
    std::function<R(Args...)>&& functor)
{
    return std::make_unique<function_wrapper<R, Args...>>(std::move(functor));
}

template <typename R, typename... Args>
std::string arguments_to_string(const std::function<R(Args...)>& f)
{
    return detail::arguments_to_string(f, std::index_sequence_for<Args...>{});
}

} // namespace clbind
