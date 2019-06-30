#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <functional>

namespace clbind
{

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

private:
    std::unordered_map<std::string, std::shared_ptr<Package>> m_registry;
};

} // namespace clbind

extern "C" {
bool registerPackage(const char* name, void (*RegFunc)(clbind::Package&));
}
