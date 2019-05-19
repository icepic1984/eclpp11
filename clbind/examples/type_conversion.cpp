#include <iostream>
#include <clbind/type_conversion.hpp>

int main()
{
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

    clbind::convert_to_lisp<int> bla;

    // clbind::convert_to_lisp<int*> blup;
    // int i;
    // blup.operator()<int>(&i);
    // std::cout << bla(10) << std::endl;
    //    clbind::convert_to_lisp<int> bla;
}
