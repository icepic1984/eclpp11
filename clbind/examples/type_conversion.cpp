#include <iostream>
#include <clbind/type_conversion.hpp>

int funcptr(int a, int b)
{
    return a + b;
}

struct functor
{
    int operator()(int a)
    {
        return a;
    }
};

struct member
{
    int test(int a, int b)
    {
        return a + b;
    }
};

int main(int argc, char** args)
{
    cl_boot(argc, args);
    struct A
    {
    };
    struct B
    {
    };
    clbind::class_registry reg;
    reg.add(typeid(A), "A");
    reg.add(typeid(B), "B");

    std::cout << reg.name(typeid(A)) << std::endl;
    std::cout << reg.name(typeid(B)) << std::endl;

    std::cout << clbind::static_type_mapping<char>::lisp_type() << std::endl;
    std::cout << clbind::static_type_mapping<uint8_t>::lisp_type() << std::endl;
    std::cout << clbind::static_type_mapping<unsigned char>::lisp_type()
              << std::endl;

    std::cout << clbind::static_type_mapping<int>::lisp_type() << std::endl;
    std::cout << clbind::static_type_mapping<int32_t>::lisp_type() << std::endl;
    std::cout << clbind::static_type_mapping<uint32_t>::lisp_type()
              << std::endl;

    std::cout << clbind::static_type_mapping<int8_t>::lisp_type() << std::endl;

    // clbind::convert_to_lisp<int> bla;

    std::cout << "f1" << std::endl;
    clbind::cpp_function f1(funcptr);
    std::cout << "f2" << std::endl;
    clbind::cpp_function f2(functor{});
    std::cout << "f3" << std::endl;
    clbind::cpp_function f3([](int a) { return a; });
    std::cout << "f4" << std::endl;
    int a = 10;
    double tmp = 20.0;
    clbind::cpp_function f4([a, tmp](int b) { return a + b + tmp; });

    std::cout << "f5" << std::endl;
    member m;
    clbind::cpp_function f5(member::test);
    std::cout << "Done" << std::endl;

    int ecl_a = 10;
    int& ecl_ref = ecl_a;

    struct Ecl_A
    {
    };

    Ecl_A ecl_struct;
    clbind::to_ecl(ecl_a);
    clbind::to_ecl(&ecl_a);
    clbind::to_ecl(ecl_ref);
    clbind::to_ecl(10);
    clbind::to_ecl(ecl_struct);
    clbind::to_ecl(&ecl_struct);
    clbind::to_ecl(Ecl_A{});
    clbind::to_ecl(ecl_ref);

    // lisp
    // (a 10 20)

    // (cl_object:func_a cl_object:int_10 cl_object:int_20)
    //
    // clbind::convert_to_lisp<int*> blup;
    // int i;
    // blup.operator()<int>(&i);
    // std::cout << bla(10) << std::endl;
    //    clbind::convert_to_lisp<int> bla;
}
