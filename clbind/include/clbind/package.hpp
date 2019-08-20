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

    template <typename F>
    void defun(const std::string& name, F&& func)
    {
        defunImpl(name, std::forward<F>(func));
    }

private:
    template <typename Return, typename... Args>
    void defunImpl(const std::string& name, Return (*func)(Args...))
    {
        std::function<Return(Args...)> function(
            std::forward<decltype(func)>(func));
    }

    // For function member point
    template <typename Return, typename Class, typename... Args>
    void defunImpl(const std::string& name, Return (Class::*func)(Args...))
    {
        defunImpl<Return, Class, Args...>(name, func);
    }

    // For const function member point
    template <typename Return, typename Class, typename... Args>
    void defunImpl(
        const std::string& name, Return (Class::*func)(Args...) const)
    {
        defunImpl<Return, const Class, Args...>(name, func);
    }

    // For functor with const member function
    template <typename Return, typename Class, typename... Args>
    void defunImpl(const std::string& name, Class&& functor,
        Return (Class::*func)(Args...) const)
    {
        defunImpl<Return, const Class, Args...>(
            name, std::forward<Class>(functor), func);
    }

    // For functor with non constant member function
    template <typename Return, typename Class, typename... Args>
    void defunImpl(
        const std::string& name, Class&& functor, Return (Class::*)(Args...))
    {
        std::function<Return(Args...)> f(std::forward<Class>(functor));
    }

    // For functor (dispatches to const vs non const overload)
    template <typename Functor>
    void defunImpl(const std::string& name, Functor&& functor)
    {
        defunImpl(name, std::forward<Functor>(functor), &Functor::operator());
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

extern "C" {

bool register_package(
    const char* name, void (*register_callback)(clbind::package&))
{
    auto package = clbind::registry::get_registry().create_package(name);
    // register_callback(package);
    package.defun("blup", [](int a, int c) { return a; });
    package.defun("blup", [&a](int b) { return a + b; });
    package.defun("blup", [&a](int b) mutable { return a + b; });

    package.defun("blup", functor{});
    package.defun("blup", functor_const{});
    package.defun("blup", &operator_test::test);
    package.defun("blup", &operator_test_const::test);
    // package.defun("blup", [&a](int b) { return a + b; });

    // package.defun("blup", &op2::test);
    // std::cout << "defun3 b" << b(10, 20) << std::endl;
    // std::cout << "defun3 c" << c(100) << std::endl;
    // std::cout << "defun3 d" << d(10, 20) << std::endl;
    // const op2 p;
    // std::cout << "defun3 e" << e(p, 10) << std::endl;

    // std::function<int(op2&, int)> f(&op2::test);

    // std::cout << f(p, 10) << std::endl;
    // std::cout << f(p, 10) << std::endl;
    return true;
}
}
#define CLBIND_PACKAGE extern "C" void
