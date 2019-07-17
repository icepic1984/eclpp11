#include <clbind/package.hpp>

void test3(int a)
{
}

CLBIND_PACKAGE test(clbind::Package& package)
{
    package.defun("test1", []() {});
    package.defun("test2", []() {});
    package.defun("test3", [](int a) { std::cout << a << std::endl; });
    package.defun("test4", &test3);
}

int main()
{
    registerPackage("test", &test);
}
