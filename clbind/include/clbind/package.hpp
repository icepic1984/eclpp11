#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <tuple>

namespace clbind
{

// https://github.com/Nelarius/wrenpp/blob/master/Wren%2B%2B.h
// http://stackoverflow.com/questions/17339789/how-to-call-a-function-on-all-variadic-template-args
// http://anthony.noided.media/blog/programming/c++/ruby/2016/05/12/mruby-cpp-and-template-magic.html

template <typename F>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R(Args...)>
{
    using ReturnType = R;

    constexpr static const std::size_t Arity = sizeof...(Args);

    template <std::size_t N>
    struct Argument
    {
        static_assert(N < Arity,
            "FunctionTraits error: invalid argument index parameter");
        using Type = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template <std::size_t N>
    using ArgumentType = typename Argument<N>::Type;
};

class Package
{
public:
    explicit Package(const std::string& name)
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
        addLambda(name, std::forward<LambdaT>(lambda), &LambdaT::operator());
    }

private:
    template <typename R, typename LambdaT, typename... ArgsT>
    void addLambda(const std::string& name, LambdaT&& lambda,
        R (LambdaT::*)(ArgsT...) const)
    {
        std::cout << "lambda" << std::endl;
        return defun(
            name, std::function<R(ArgsT...)>(std::forward<LambdaT>(lambda)));
    }

    std::string m_name;
};

class Registry
{
public:
    Package& createPackage(const std::string& name)
    {
        m_registry[name] = std::make_shared<Package>(name);
        return *m_registry[name];
    }

    static Registry& registry()
    {
        static Registry m_registry;
        return m_registry;
    }

private:
    Registry() = default;

    Registry(const Registry&) = delete;

    Registry operator=(const Registry) = delete;

    std::unordered_map<std::string, std::shared_ptr<Package>> m_registry;
};

} // namespace clbind

extern "C" {
bool registerPackage(const char* name, void (*regFunc)(clbind::Package&))
{
    auto package = clbind::Registry::registry().createPackage(name);
    regFunc(package);
    return true;
}
int add2(int a, int b)
{
    return a + b;
}
}

#define CLBIND_PACKAGE extern "C" void
