#include <iostream>
#include <clbind/type_conversion.hpp>

using namespace clbind;

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

int test_func(int a, int b)
{
    return a + b;
}

cl_object add(cl_object a, cl_object b)
{
    auto aI = ecl_to_int32_t(a);
    auto bI = ecl_to_int32_t(b);
    return ecl_make_int32_t(aI + bI);
}

cl_object callback(void* f, cl_arglist arglist)
{
    std::cout << "yeah" << std::endl;
    return ecl_make_int32_t(0);
}

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

    auto thunc_ptr = clbind::createFunctor(std::function(&test_func));

    std::function w(&test_func);
    clbind::FunctionWrapper wrapper(w);

    auto func_ptr = wrapper.ptr();

    typedef int (*fptr)(const void*, int, int);
    fptr my_fptr = reinterpret_cast<fptr>(thunc_ptr);
    std::cout << my_fptr(func_ptr, 10, 10) << std::endl;

    clbind::define_function("test", callback, reinterpret_cast<void*>(add));
    cl_eval(c_string_to_object("(test 10 20 30 )"));
    cl_shutdown();
    // auto thunc = reinterpret_cast<const void*>(
    //     clbind::CallFunctor<int, int, int>::apply);

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
