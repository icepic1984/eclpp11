#include <clbind/package.hpp>

int main()
{
    int a = 10;
    clbind::Registry reg;
    auto pack = reg.createPackage("test");
    pack.defun(std::string("test"), [&a]() { return a; });
    // wrapper([](int a) {});

    // std::function<void()> l = []() {};
}
