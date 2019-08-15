#pragma once

#include <clbind/convert_types.hpp>
#include <clbind/function_wrapper.hpp>
#include <clbind/ecl_utilities.hpp>
#include <clbind/print_type.hpp>
#include <clbind/function_traits.hpp>
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

    template <typename F>
    void defun2(const std::string& name, F&& func)
    {
        defun2Impl(name, std::forward<F>(func));
    }

    template <typename F>
    auto defun3(const std::string& name, F&& func) -> decltype(auto)
    {
        // typename function_traits<F>::func_type f(std::forward<F>(func));
        // typename function_traits<F>::func_type a;
        // static_assert(std::is_same_v<typename function_traits<F>::func_type,
        //     std::function<int(int)>>);
        typename as_function<function_return_type_t<F>,
            function_args_t<F>>::type f(std::forward<F>(func));
        return f;
        // return typename function_traits<F>::func_type f(std::forward(func));
    }

private:
    template <typename Return, typename... Args>
    void defun2Impl(const std::string& name, Return (*func)(Args...))
    {
        std::function<Return(Args...)> function(
            std::forward<decltype(func)>(func));
        std::cout << function(10, 20) << std::endl;
    }

    template <typename Class, typename Return, typename... Args>
    void defun2Impl(const std::string& name, Return (Class::*func)(Args...))
    {
        defun2Impl<Return(Class&, Args...)>(
            name, std::forward<decltype(func)>(func));
    }

    template <typename R, typename Class, typename... Args>
    void defun2Impl(
        const std::string& name, Class&& functor, R (Class::*)(Args...) const)
    {
    }

    template <typename Functor>
    void defun2Impl(const std::string& name, Functor&& functor)
    {
        defun2Impl(name, std::forward<Functor>(functor), &Functor::operator());
    }

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
    LambdaT&& lambda, R (LambdaT::*)(ArgsT...))
{
    return defun2(package_name, symbol_name,
        std::function<R(ArgsT...)>(std::forward<LambdaT>(lambda)));
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

class fw
{
public:
    virtual const void* pointer() = 0;

    virtual ~fw() = default;

    fw(const fw&) = delete;

    fw& operator=(const fw&) = delete;
};

template <typename R, typename... Args>
class fwb : public fw
{
public:
    using functor_t = std::function<R(Args...)>;

    explicit fwb(const functor_t& f)
    {
        m_function = f;
    }

    const void* ptr() final
    {
        return reinterpret_cast<const void*>(&m_function);
    }

private:
    functor_t m_function;
};
} // namespace clbind

int global = 0;
int bla(int a, int b)
{
    return a + b;
}

struct op
{
    int fu = 100;
    int operator()(int a, int b)
    {
        return fu + a - b;
    }
};

struct op2
{
    int b = -100;
    int test(int a)
    {
        return a + 123 - b;
    }
};

int a = 666;
extern "C" {
void reg()
{

    clbind::defun("BLA", "BLUP2", bla);
    clbind::defun2("BLA", "BLUP3", [](int a, int b) { return a + b; });
    clbind::defun2("BLA", "BLUP4", [&a](int b, int c) { return a + b + c; });
    clbind::defun2("BLA", "BLUP5", op{});
    // clbind::defun("BLA", "BLUP", [](int a, int b) { return a + b; });

    // auto l = [](int a, int b) { return a + b; };
    // func([&global](int a, int b) mutable { return a + b; });
    //  clbind::defun("BLA", "BLUP3", [](int a, int b) { return a + b; });
}

void init()
{
    reg();
}

bool register_package(
    const char* name, void (*register_callback)(clbind::package&))
{
    auto package = clbind::registry::get_registry().create_package(name);
    // register_callback(package);
    package.defun2("bla", bla);
    package.defun2("blup", [&a](int b) { return a + b; });

    auto b = package.defun3("blup", [](int a, int c) { return a; });
    auto c = package.defun3("blup", [&a](int b) { return a + b; });
    auto d = package.defun3("blup", op{});
    auto e = package.defun3("blup", &op2::test);
    std::cout << "defun3 b" << b(10, 20) << std::endl;
    std::cout << "defun3 c" << c(100) << std::endl;
    std::cout << "defun3 d" << d(10, 20) << std::endl;
    op2 p;
    std::cout << "defun3 e" << e(p, 10) << std::endl;

    std::function<int(op2&&, int)> f(&op2::test);
    std::cout << f(op2{}, 10) << std::endl;
    std::cout << f(p, 10) << std::endl;
    return true;
}
}

#define CLBIND_PACKAGE extern "C" void
