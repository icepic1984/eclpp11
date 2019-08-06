#include <clbind/package.hpp>
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

    int ref = 3;
    const int& ref3 = ref;
    auto sdf = clbind::toEcl(5);
    auto env = ecl_process_env();
    struct ecl_stack_frame frame;
    ecl_stack_frame_open(env, (cl_object)&frame, 2);

    ecl_stack_frame_push((cl_object)&frame, clbind::toEcl(5));
    ecl_stack_frame_push((cl_object)&frame, clbind::toEcl(10));
    ecl_stack_frame_close((cl_object)&frame);

    std::cout << (t_frame == ecl_t_of((cl_object)&frame)) << std::endl;

    std::cout << clbind::toCpp<int>(sdf);
}
