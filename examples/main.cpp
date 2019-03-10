#include <eclpp.hpp>
#include <iostream>

int main()
{
    auto bla = eclpp::convert<int>::to_ecl(10);
    std::cout << eclpp::convert<int>::to_cpp(bla) << std::endl;

    // auto blup = eclpp::convert<double>::to_ecl(1.0);
    // std::cout << eclpp::convert<double>::to_cpp(blup) << std::endl;

    cl_object b = ecl_make_long_float(10.0);
    double d = ecl_to_long_double(b);
    std::cout << d << std::endl;
}
