#include <clbind/clbind.hpp>
#include <ecl/internal.h>
#include <type_traits>
#include <functional>

int func_pointer(int a, int b)
{
    return a + b;
}

struct functor
{
    int fu = 100;
    int operator()(int a, int b)
    {
        return fu + a - b;
    }
};

struct functor_const
{
    int fu = 100;
    int operator()(int a, int b) const
    {
        return fu + a - b;
    }
};

struct operator_test
{
    int b = -100;
    int test(int a)
    {
        b = 10;
        return a + 123 - b;
    }
};

struct operator_test_const
{
    int b = -100;
    int test(int a) const
    {
        return a + 123 - b;
    }
};

static int a = 0;
static const operator_test_const otc;
static operator_test ot;

CLBIND_PACKAGE test(clbind::package& package)
{
    std::cout << "Start test" << std::endl;
    int* p = new int(10);
    package.defun("BLUP1", [](int a, int c) { return a; });
    package.defun("BLUP2", [&a](int b) { return a + b; });
    package.defun("BLUP3", [&a](int b) mutable { return a + b; });
    package.defun("BLUP4", functor{});
    package.defun("BLUP5", functor_const{});
    // package.defun2("BLUP6", &operator_test::test);
    // package.defun2("BLUP7", &operator_test_const::test);
    package.defun("BLUP8", func_pointer);
    package.defun("BLUP9", &func_pointer);
    package.defun("BLUP10", [p](int a) { *p += a; });
    package.defun("BLUP11", [p]() { return *p; });
    package.defun("BLA1", [&a]() { ++a; });
    package.defun("BLA2", [&a]() { return a; });
    std::cout << "End test" << std::endl;
}

int main(int argc, char** args)
{

    cl_boot(argc, args);
    register_package("TEST2", &test);
    cl_safe_eval(ecl_read_from_cstring_safe("(test2:blup1 10 20)", ECL_NIL),
        ECL_NIL, ECL_NIL);

    // struct ecl_stack_frame frame;
    // auto env = ecl_process_env();
    // auto name = ecl_stack_frame_open(env, (cl_object)&frame, 0);
    // ecl_stack_frame_push(name, clbind::to_ecl(5));
    // ecl_stack_frame_push(name, clbind::to_ecl(6));
    // ecl_stack_frame_push(name, clbind::to_ecl(7));
    // ecl_stack_frame_close(name);

    // auto b = clbind::wrap(cool2, name);
    // std::cout << "wrap" << clbind::to_cpp<int>(b) << std::endl;

    // auto c = clbind::wrap(cool2, name);
    // std::cout << "wrap2" << clbind::to_cpp<int>(c) << std::endl;

    // register_package("test", nullptr);
}
