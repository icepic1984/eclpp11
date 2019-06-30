#include <clbind/package.hpp>

CLBIND_PACKAGE test(clbind::Package& package)
{
    package.defun("test1", []() {});
    package.defun("test2", []() {});
}

int main()
{
    registerPackage("test", &test);
}
