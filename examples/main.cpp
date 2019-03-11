#include <eclpp.hpp>
#include <iostream>

template <typename T>
void test_conversion(T input)
{
    auto obj = eclpp::convert<T>::to_ecl(input);
    auto output = eclpp::convert<T>::to_cpp(obj);
    static_assert(sizeof(input) == sizeof(output));
    assert(input == output);
}

int main(int argc, char** args)
{
    cl_boot(argc, args);
    test_conversion(10.0);
    test_conversion(10.0f);
    test_conversion(std::uint8_t(10));
}
