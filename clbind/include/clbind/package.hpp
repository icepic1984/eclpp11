#pragma once

#include <clbind/convert_types.hpp>
#include <clbind/function_wrapper.hpp>
#include <clbind/ecl_utilities.hpp>
#include <clbind/print_type.hpp>
#include <clbind/function_traits.hpp>
#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>
#include <tuple>
#include <ecl/ecl.h>

namespace clbind
{

class package
{
public:
    explicit package(const std::string& name)
        : m_name(name){};

    template <typename F>
    decltype(auto) defun(const std::string& name, F&& func)
    {
        using func_type =
            as_function_t<function_return_type_t<F>, function_args_t<F>>;

        return func_type(std::forward<F>(func));
    }

    template <typename F>
    void defun2(const std::string& name, F&& func)
    {
        using func_type =
            as_function_t<function_return_type_t<F>, function_args_t<F>>;

        m_function_table.insert(std::make_pair(
            name, make_function_wrapper(func_type(std::forward<F>(func)))));

        static auto callback = +[](void* f, cl_object arglist) {
            return wrap2(*reinterpret_cast<func_type*>(f), arglist);
        };

        define_function("TEST", name.c_str(), callback,
            static_cast<void*>(m_function_table[name]->pointer()));
    }

private:
    using function_t = std::unique_ptr<function_wrapper_base>;

    std::string m_name;

    std::unordered_map<std::string, function_t> m_function_table;
};

class registry
{
public:
    package& create_package(const std::string& name)
    {
        m_registry[name] = std::make_shared<package>(name);
        return *m_registry[name];
    }

    static registry& get_registry()
    {
        static registry m_registry;
        return m_registry;
    }

private:
    registry() = default;

    registry(const registry&) = delete;

    registry operator=(const registry) = delete;

    std::unordered_map<std::string, std::shared_ptr<package>> m_registry;
};

} // namespace clbind

int func_pointer(int a, int b)
{
    return a + b;
}

struct functor
{
    int fu = 100;
    int operator()(int a, int b)
    {
        return fu + a - b;
    }
};

struct functor_const
{
    int fu = 100;
    int operator()(int a, int b) const
    {
        return fu + a - b;
    }
};

struct operator_test
{
    int b = -100;
    int test(int a)
    {
        b = 10;
        return a + 123 - b;
    }
};

struct operator_test_const
{
    int b = -100;
    int test(int a) const
    {
        return a + 123 - b;
    }
};

int a = 0;
const operator_test_const otc;
operator_test ot;

extern "C" {

bool register_package(
    const char* name, void (*register_callback)(clbind::package&))
{
    const operator_test_const otc;
    operator_test ot;

    auto& package = clbind::registry::get_registry().create_package(name);
    // register_callback(package);
    package.defun("blup1", [](int a, int c) { return a; });
    package.defun("blup2", [&a](int b) { return a + b; });
    package.defun("blup3", [&a](int b) mutable { return a + b; });
    package.defun("blup4", functor{});
    package.defun("blup5", functor_const{});
    package.defun("blup6", &operator_test::test);
    package.defun("blup7", &operator_test_const::test);
    package.defun("blup8", func_pointer);
    package.defun("blup9", &func_pointer);

    return true;
}

bool reg()

{
    auto& package = clbind::registry::get_registry().create_package("TEST");
    // register_callback(package);
    package.defun2("BLUP1", [](int a, int c) { return a; });
    package.defun2("BLUP2", [&a](int b) { return a + b; });
    package.defun2("BLUP3", [&a](int b) mutable { return a + b; });
    package.defun2("BLUP4", functor{});
    package.defun2("BLUP5", functor_const{});
    // package.defun2("BLUP6", &operator_test::test);
    // package.defun2("BLUP7", &operator_test_const::test);
    package.defun2("BLUP8", func_pointer);
    package.defun2("BLUP9", &func_pointer);

    return true;
}
}
#define CLBIND_PACKAGE extern "C" void
