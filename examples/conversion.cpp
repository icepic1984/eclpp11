#include <eclpp.hpp>
#include <iostream>

template <typename T>
void test_conversion(T input)
{
    auto obj = eclpp::to_ecl(input);
    auto output = eclpp::to_cpp<T>(obj);
    static_assert(sizeof(input) == sizeof(output));
    assert(input == output);
}

int main(int argc, char** args)
{
    cl_boot(argc, args);
    test_conversion(10.0);
    test_conversion(10.0f);
    test_conversion(std::uint8_t(10));
    test_conversion(std::int8_t(10));
    test_conversion(std::int16_t(10));
    test_conversion(std::uint16_t(10));
    test_conversion(std::int32_t(10));
    test_conversion(std::uint32_t(10));
    test_conversion(std::int64_t(10));
    test_conversion(std::uint64_t(10));

    eclpp::val v1(10.0f);
    eclpp::val v2(10.0);
}
