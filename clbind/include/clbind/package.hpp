#pragma once

#include <clbind/convert_types.hpp>
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

} // namespace clbind

extern "C" {
bool register_package(
    const char* name, void (*register_callback)(clbind::package&))
{
    auto package = clbind::registry::get_registry().create_package(name);
    register_callback(package);
    return true;
}
}

#define CLBIND_PACKAGE extern "C" void
