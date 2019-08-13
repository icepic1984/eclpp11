#pragma once

#include <clbind/convert_types.hpp>
#include <clbind/function_wrapper.hpp>
#include <clbind/ecl_utilities.hpp>
#include <string>
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

    template <typename R, typename... Args>
    void defun(const std::string& name, std::function<R(Args...)> f)
    {
        std::cout << "Name of fun: " << name << std::endl;
    }

    template <typename R, typename... Args>
    void defun(const std::string& name, R (*f)(Args...))
    {
        defun(name, std::function<R(Args...)>(f));
    }

    template <typename LambdaT>
    void defun(const std::string& name, LambdaT&& lambda)
    {
        add_lambda(name, std::forward<LambdaT>(lambda), &LambdaT::operator());
    }

private:
    template <typename R, typename LambdaT, typename... ArgsT>
    void add_lambda(const std::string& name, LambdaT&& lambda,
        R (LambdaT::*)(ArgsT...) const)
    {
        std::cout << "lambda" << std::endl;
        return defun(
            name, std::function<R(ArgsT...)>(std::forward<LambdaT>(lambda)));
    }

    std::string m_name;
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

template <typename T>
void print_type_in_compilation_error(T&&)
{
    static_assert(std::is_same<T, int>::value && !std::is_same<T, int>::value,
        "Compilation failed because you wanted to read the type. See below");
}

template <typename F>
void defun(const char* package_name, const char* symbol_name, F f)
{
    static auto callback = +[](void* f, cl_object arglist) {
        return wrap(reinterpret_cast<F>(f), arglist);
    };

    define_function(
        package_name, symbol_name, callback, reinterpret_cast<void*>(f));
}

template <typename R, typename... Args>
void defun2(const char* package_name, const char* symbol_name,
    std::function<R(Args...)> f)
{
    std::function<R(Args...)>* tmp = new std::function<R(Args...)>(f);
    auto callback = +[](void* f2, cl_object argslist) {
        return wrap2(
            *reinterpret_cast<std::function<R(Args...)>*>(f2), argslist);
    };
    define_function(
        package_name, symbol_name, callback, reinterpret_cast<void*>(tmp));
}

template <typename R, typename LambdaT, typename... ArgsT>
void add_lambda(const char* package_name, const char* symbol_name,
    LambdaT&& lambda, R (LambdaT::*)(ArgsT...) const)
{
    return defun2(package_name, symbol_name,
        std::function<R(ArgsT...)>(std::forward<LambdaT>(lambda)));
}

template <typename LambdaT>
void defun2(const char* package_name, const char* symbol_name, LambdaT&& lambda)
{
    add_lambda(package_name, symbol_name, std::forward<LambdaT>(lambda),
        &LambdaT::operator());
}

} // namespace clbind

int global = 0;
int bla(int a, int b)
{
    return a + b;
}

template <typename F>
void func(F f)
{
    static_assert(clbind::function_traits<F>::size == 2);
    static_assert(
        std::is_same_v<clbind::function_args_t<F>, std::tuple<int, int>>);
}

int a = 666;
void reg()
{
    clbind::defun("BLA", "BLUP2", bla);
    clbind::defun2("BLA", "BLUP3", [](int a, int b) { return a + b; });
    clbind::defun2("BLA", "BLUP4", [&a](int b, int c) { return a + b + c; });
    // clbind::defun("BLA", "BLUP", [](int a, int b) { return a + b; });

    // auto l = [](int a, int b) { return a + b; };
    // func([&global](int a, int b) mutable { return a + b; });
    //  clbind::defun("BLA", "BLUP3", [](int a, int b) { return a + b; });
}
extern "C" {

void init()
{
    reg();
}

bool register_package(
    const char* name, void (*register_callback)(clbind::package&))
{
    auto package = clbind::registry::get_registry().create_package(name);
    register_callback(package);
    return true;
}
}

#define CLBIND_PACKAGE extern "C" void
