#include <clbind/package.hpp>
#include <clbind/function_traits.hpp>
#include <ecl/internal.h>
#include <type_traits>
#include <functional>

CLBIND_PACKAGE test(clbind::Package& package)
{
    package.defun("test1", []() {});
    package.defun("test2", []() {});
    package.defun("test3", [](int a) { std::cout << a << std::endl; });
}

void f(int a, double b)
{
}

int g(void)
{
}

int j(int a, double b)
{
}

/** Object to use for returning multiple values. */
struct return_stack
{
    /** The type of value that a C function must return. */
    /** The creator with no values. */
    return_stack()
        : env(ecl_process_env())
    {
        env->nvalues = 0;
        env->values[0] = ECL_NIL;
    }
    /** A function to add one value at a time. */
    return_stack& operator<<(cl_object o)
    {
        env->values[env->nvalues++] = o;
        return *this;
    }
    /** A function to finally return all the values that were grouped. */
    cl_object return_value()
    {
        return env->values[0];
    }

private:
    /* Copy constructors are hidden and forbidden. */
    return_stack(const return_stack& s);
    const return_stack& operator=(const return_stack& s);
    const cl_env_ptr env;
};

int cool(int a)
{
    return ++a;
}

int cool2(int a, int b, int c)
{
    return a + b + c;
}
void va(cl_narg narg, ...)
{

    ECL_STACK_FRAME_VARARGS_BEGIN(narg, narg, frame);
    {
        std::cout << ecl_t_of(clbind::nth_arg(frame, 2)) << std::endl;
        std::cout << ecl_t_of(clbind::nth_arg(frame, 1)) << std::endl;
        std::cout << clbind::toCpp<int>(clbind::nth_arg(frame, 1)) << std::endl;
        std::cout << clbind::toCpp<int>(clbind::nth_arg(frame, 2)) << std::endl;
    }
    ECL_STACK_FRAME_VARARGS_END(frame);
}

int ft_test(int a, int b, int c)
{
}

struct ft_op
{
    int operator()(int a, double b)
    {
        return 0;
    }
};

struct ft_member
{
    typedef int (*Operation)(int a, int b);

    int test(int a, double b)
    {
        return 0;
    }

    Operation b;

    int c;
};

// template <typename F>
// using function_args_t = boost::callable_traits::args_t<F, std::tuple>;

template <typename F>
using function_traits_t = typename clbind::function_traits<F>::args;

template <typename T>
struct fill_tuple;

template <typename... Args>
struct fill_tuple<std::tuple<Args...>>
{
    template <typename F>
    static constexpr cl_object wrap(F&& f, cl_object frame)
    {
        wrapImpl2(
            std::forward<F>(f), std::index_sequence_for<Args...>{}, frame);
    }

private:
    template <typename F, std::size_t... Index>
    static constexpr cl_object wrapImpl2(
        F&& f, std::index_sequence<Index...>, cl_object frame)
    {
        using return_type = typename clbind::function_traits<F>::return_type;

        if constexpr (std::is_same_v<void, return_type>)
        {

            std::invoke(f, clbind::toCpp<std::tuple_element_t<Index, Tuple>>(
                               clbind::nth_arg(frame, Index))...);
            return ECL_NIL;
        }
        else
        {
            return clbind::toEcl<int>(std::invoke(
                f, clbind::toCpp<std::tuple_element_t<Index, Tuple>>(
                       clbind::nth_arg(frame, Index))...));
        }
    }

    using Tuple = std::tuple<Args...>;
};

template <typename F>
cl_object wrap2(F&& func, cl_object frame)
{
    fill_tuple<function_traits_t<F>>::wrap(std::forward<F>(func), frame);
}

template <typename F>
int ft(F&& f)
{
    // static_assert(std::is_same_v<std::tuple_element_t<0, function_args_t<F>>,
    //     std::add_lvalue_reference_t<ft_member>>);

    static_assert(std::is_same_v<std::tuple_element_t<0, function_traits_t<F>>,
        std::add_lvalue_reference_t<ft_member>>);
}

int main(int argc, char** args)
{

    cl_boot(argc, args);

    static_assert(
        std::is_same_v<clbind::FunctionTraits<decltype(j)>::ArgumentType<0>,
            int>);
    static_assert(
        std::is_same_v<clbind::FunctionTraits<decltype(j)>::ArgumentType<1>,
            double>);
    static_assert(
        std::is_same_v<clbind::FunctionTraits<decltype(f)>::ReturnType, void>);

    auto bla = clbind::detail::Convert<int>::toEcl(10);
    std::cout << clbind::detail::Convert<int>::toCpp(bla) << std::endl;

    auto blup = clbind::detail::Convert<float>::toEcl(10.01234f);
    std::cout << clbind::detail::Convert<float>::toCpp(blup) << std::endl;

    auto blup2 = clbind::detail::Convert<double>::toEcl(10.01234);
    std::cout << clbind::detail::Convert<double>::toCpp(blup2) << std::endl;

    //   va(2, clbind::toEcl(10), clbind::toEcl(20));

    // auto b = cl_cons(clbind::toEcl(5), ECL_NIL);
    // std::cout << ecl_t_of(b) << std::endl;

    // auto b1 = clbind::nth_arg(b, 1);
    // std::cout << ecl_t_of(b1) << std::endl;

    // Setup stack
    auto env = ecl_process_env();
    struct ecl_stack_frame frame;
    auto name = ecl_stack_frame_open(env, (cl_object)&frame, 0);
    ecl_stack_frame_push(name, clbind::toEcl(5));
    ecl_stack_frame_push(name, clbind::toEcl(6));
    ecl_stack_frame_push(name, clbind::toEcl(7));
    ecl_stack_frame_close(name);

    auto b = clbind::wrap(cool2, name);
    std::cout << "wrap" << clbind::toCpp<int>(b) << std::endl;

    auto c = wrap2(cool2, name);
    std::cout << "wrap2" << clbind::toCpp<int>(c) << std::endl;

    // Use stack
    // std::cout << clbind::toCpp<int>(clbind::nth_arg(name, 1)) << std::endl;
    // auto b = clbind::wrap(cool, name);
    // std::cout << clbind::toCpp<int>(b) << std::endl;

    // ft(&ft_test);
    // ft_member m;
    // ft(ft_member::test);
    //    ft(ft_op{});
}
