#include <clbind/package.hpp>
#include <ecl/internal.h>
#include <type_traits>

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

int main(int argc, char** args)
{
    cl_boot(argc, args);
    clbind::FunctionTraits<decltype(j)> t1{};
    clbind::FunctionTraits<decltype(g)> t2{};
    clbind::FunctionTraits<decltype(f)> t3{};

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

    va(2, clbind::toEcl(10), clbind::toEcl(20));

    // auto b = cl_cons(clbind::toEcl(5), ECL_NIL);
    // std::cout << ecl_t_of(b) << std::endl;

    // auto b1 = clbind::nth_arg(b, 1);
    // std::cout << ecl_t_of(b1) << std::endl;

    // Setup stack
    auto env = ecl_process_env();
    struct ecl_stack_frame frame;
    auto name = ecl_stack_frame_open(env, (cl_object)&frame, 0);
    ecl_stack_frame_push(name, clbind::toEcl(5));
    //    ecl_stack_frame_push(name, clbind::toEcl(6));
    ecl_stack_frame_close(name);

    // Use stack
    std::cout << clbind::toCpp<int>(clbind::nth_arg(name, 1)) << std::endl;
    auto b = clbind::wrap(cool, name);
    std::cout << clbind::toCpp<int>(b) << std::endl;
}
