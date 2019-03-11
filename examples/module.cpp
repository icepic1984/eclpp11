#include <eclpp.hpp>
#include <iostream>

struct Image
{
    Image()
    {
        std::cout << "Construct" << std::endl;
    }

    ~Image()
    {
        std::cout << "Destruct" << std::endl;
    }
    int width;
    int height;
};

cl_object make_image()
{
    return eclpp::make_foreign<Image>();
}

extern "C" {
void init_lib(void)
{
    // https://stackoverflow.com/questions/44494613/embeddable-common-lisp-ecl-load-shared-library-with-extension-functionsxo
    std::cout << "Initialize library" << std::endl;
    ecl_def_c_function(ecl_read_from_cstring("make-image"),
        reinterpret_cast<cl_objectfn_fixed>(make_image), 0);
}
}
