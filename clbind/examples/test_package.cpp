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

int main()
{
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
}
