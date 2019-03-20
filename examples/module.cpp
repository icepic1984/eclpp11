#include <eclpp.hpp>
#include <iostream>

class Image
{
public:
    Image()
        : width(0)
        , height(0)
    {
        std::cout << "Construct" << std::endl;
    }

    ~Image()
    {
        std::cout << "Destruct" << std::endl;
    }

    Image(int w, int h)
        : width(w)
        , height(h)
    {
        std::cout << "Construct: " << width << " " << height << std::endl;
    }

    int getWidth()
    {
        return width;
    }

    int getHeight()
    {
        return height;
    }

    void setWidth(int w)
    {
        width = w;
    }

    void setHeight(int h)
    {
        height = h;
    }

    int width;
    int height;
};

struct Parameter
{
    uint32_t x;
    uint32_t y;
    double d;
};

void initializeParameter(int size, Parameter* parameter, int x, int y, double d)
{
    for (int i = 0; i < size; ++i)
    {
        parameter[i].x = x;
        parameter[i].y = y;
        parameter[i].d = d;
    }
}

void printParameter(int size, const Parameter* parameter)
{
    for (int i = 0; i < size; ++i)
    {
        std::cout << "x: " << parameter[i].x << " y: " << parameter[i].y
                  << "d: " << parameter[i].d << std::endl;
    }
}

// https://stackoverflow.com/questions/44494613/embeddable-common-lisp-ecl-load-shared-library-with-extension-functionsxo
extern "C" {
void init_lib(void)
{
    std::cout << "Initialize library" << std::endl;
    eclpp::type<Image>("image")
        .constructor()
        .define("get-width", &Image::getWidth)
        .define("set-width", &Image::setWidth)
        .define("get-height", &Image::getHeight)
        .define("set-height", &Image::setHeight);
    eclpp::group("test")
        .define("initialize-parameter", &initializeParameter)
        .define("print-paramater", &printParameter);
}
}
