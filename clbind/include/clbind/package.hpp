#pragma once

#include <clbind/convert_types.hpp>
#include <clbind/function_wrapper.hpp>
#include <clbind/ecl_utilities.hpp>
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

class function
{
public:
    using function_wrapper_t = std::unique_ptr<function_wrapper_base>;

    explicit function(const std::string& name,
        const std::string& return_type_string,
        const std::string& argument_type_string,
        function_wrapper_t&& function_wrapper)
        : m_name(name)
        , m_return_type_string(return_type_string)
        , m_argument_type_string(argument_type_string)
        , m_function_wrapper(std::move(function_wrapper))
    {
    }

    std::string name() const
    {
        return m_name;
    }

    std::string argument_type_string() const
    {
        return m_argument_type_string;
    }

    std::string return_type_string()
    {
        return m_return_type_string;
    }

    void* pointer()
    {
        return m_function_wrapper->pointer();
    }

private:
    std::string m_name;

    std::string m_return_type_string;

    std::string m_argument_type_string;

    function_wrapper_t m_function_wrapper;
};

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

        m_function_table.insert(std::make_pair(name,
            function(name, clbind::to_type_string<function_return_type_t<F>>(),
                clbind::arguments_to_string(func_type{}),
                make_function_wrapper(func_type(std::forward<F>(func))))));

        std::cout << "args" << clbind::arguments_to_string(func_type{})
                  << std::endl;
        std::cout << "return: "
                  << clbind::to_type_string<function_return_type_t<F>>()
                  << std::endl;

        static auto callback = +[](void* f, cl_object arglist) {
            return wrap2(*reinterpret_cast<func_type*>(f), arglist);
        };

        define_function(m_name.c_str(), name.c_str(), callback,
            static_cast<void*>(m_function_table.at(name).pointer()));
    }

private:
    std::string m_name;

    std::unordered_map<std::string, function> m_function_table;

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
}
#define CLBIND_PACKAGE extern "C" void
