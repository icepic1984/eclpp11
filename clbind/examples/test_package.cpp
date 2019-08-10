#include <clbind/clbind.hpp>
#include <ecl/internal.h>
#include <type_traits>
#include <functional>

CLBIND_PACKAGE test(clbind::package& package)
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
        std::cout << clbind::to_cpp<int>(clbind::nth_arg(frame, 1))
                  << std::endl;
        std::cout << clbind::to_cpp<int>(clbind::nth_arg(frame, 2))
                  << std::endl;
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

int main(int argc, char** args)
{

    cl_boot(argc, args);

    struct ecl_stack_frame frame;
    auto env = ecl_process_env();
    auto name = ecl_stack_frame_open(env, (cl_object)&frame, 0);
    ecl_stack_frame_push(name, clbind::to_ecl(5));
    ecl_stack_frame_push(name, clbind::to_ecl(6));
    ecl_stack_frame_push(name, clbind::to_ecl(7));
    ecl_stack_frame_close(name);

    auto b = clbind::wrap(cool2, name);
    std::cout << "wrap" << clbind::to_cpp<int>(b) << std::endl;

    auto c = clbind::wrap(cool2, name);
    std::cout << "wrap2" << clbind::to_cpp<int>(c) << std::endl;
}
