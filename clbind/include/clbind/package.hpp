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
        : m_name(name)
    {
        make_package(m_name.c_str());
    };

    ~package() noexcept
    {
        if (!m_moved_from)
        {
            delete_package(m_name.c_str());
        }
    }

    package(package&& other) noexcept
        : m_name(std::move(other.m_name))
        , m_function_table(std::move(other.m_function_table))

    {
        other.m_moved_from = true;
    }

    package& operator=(package&& other) noexcept
    {
        m_name = std::move(other.m_name);
        m_function_table = std::move(other.m_function_table);
        other.m_moved_from = true;
        return *this;
    }

    template <typename F>
    void defun(const std::string& name, F&& func)
    {
        using func_type =
            as_function_t<function_return_type_t<F>, function_args_t<F>>;

        m_function_table.insert(std::make_pair(
            name, make_function_wrapper(func_type(std::forward<F>(func)))));

        static auto callback = +[](void* f, cl_object arglist) {
            return wrap2(*reinterpret_cast<func_type*>(f), arglist);
        };

        define_function(m_name.c_str(), name.c_str(), callback,
            static_cast<void*>(m_function_table[name]->pointer()));
    }

private:
    using function_t = std::unique_ptr<function_wrapper_base>;

    std::string m_name;

    std::unordered_map<std::string, function_t> m_function_table;

    bool m_moved_from = false;
};

class registry
{
public:
    package& create_package(const std::string& name)
    {
        if (m_registry.find(name) != m_registry.end())
        {
            std::stringstream ss;
            ss << "The package \"" << name << "\" already exists.";
            throw std::runtime_error(ss.str());
        }
        m_registry.emplace(std::make_pair(name, package{name}));

        return m_registry.at(name);
    }

    package& get_package(const std::string& name)
    {
        if (m_registry.find(name) == m_registry.end())
        {
            std::stringstream ss;
            ss << "The package \"" << name << "\" does not exists.";
            throw std::runtime_error(ss.str());
        }
        return m_registry.at(name);
    }

    void delete_package(const std::string& name)
    {
        if (auto iter = m_registry.find(name); iter == m_registry.end())
        {
            std::stringstream ss;
            ss << "The package \"" << name << "\" does not exists.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            m_registry.erase(iter);
        }
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

    std::unordered_map<std::string, package> m_registry;
};

} // namespace clbind

extern "C" {

bool register_package(
    const char* name, void (*register_callback)(clbind::package&))
{
    auto& package = clbind::registry::get_registry().create_package(name);
    register_callback(package);
    return true;
}

bool delete_package(const char* name)
{
    clbind::registry::get_registry().delete_package(name);
    return true;
}

bool reg()

{
    // auto& package = clbind::registry::get_registry().create_package("TEST");
    // // register_callback(package);
    // package.defun("BLUP1", [](int a, int c) { return a; });
    // package.defun("BLUP2", [&a](int b) { return a + b; });
    // package.defun("BLUP3", [&a](int b) mutable { return a + b; });
    // package.defun("BLUP4", functor{});
    // package.defun("BLUP5", functor_const{});
    // // package.defun2("BLUP6", &operator_test::test);
    // // package.defun2("BLUP7", &operator_test_const::test);
    // package.defun("BLUP8", func_pointer);
    // package.defun("BLUP9", &func_pointer);
    // package.defun("BLA1", [&a]() { ++a; });
    // package.defun("BLA2", [&a]() { return a; });
    // clbind::registry::get_registry().delete_package("TEST");

    // return true;
}
}
#define CLBIND_PACKAGE extern "C" void
