#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <tuple>

#include <ecl/ecl.h>

namespace clbind
{

namespace detail
{
template <typename T, typename Enable = void>
struct Convert;

template <>
struct Convert<int>
{
    static int toCpp(cl_object v)
    {
        return ecl_to_int(v);
    }

    static cl_object toEcl(int v)
    {
        return ecl_make_int(v);
    }
};

template <>
struct Convert<float>
{
    static float toCpp(cl_object v)
    {
        return ecl_to_float(v);
    }

    static cl_object toEcl(float v)
    {
        return ecl_make_single_float(v);
    }
};

template <>
struct Convert<double>
{
    static double toCpp(cl_object v)
    {
        return ecl_to_double(v);
    }

    static cl_object toEcl(double v)
    {
        return ecl_make_double_float(v);
    }
};

} // namespace detail

template <typename T>
decltype(auto) toCpp(cl_object v)
{
    return clbind::detail::Convert<std::decay_t<T>>::toCpp(v);
}

template <typename T>
decltype(auto) toEcl(T&& v)
{
    return clbind::detail::Convert<std::decay_t<decltype(v)>>::toEcl(
        std::forward<T>(v));
}

cl_object nth_arg(cl_object arglist, int i)
{
    if (i >= arglist->frame.size)
    {
        std::cerr << "Missing argument #" << i
                  << " in a CL-CXX wrapped function.\n";
        abort();
    }
    return ((cl_object*)arglist->frame.base)[i];
}

/** Object to use for returning multiple values. */
struct return_stack
{
    /** The type of value that a C function must return. */
    /** The creator with no values. */
    return_stack()
        : env(ecl_process_env())
    {
        env->nvalues = 0;
        env->values[0] = ECL_NIL;
    }
    /** A function to add one value at a time. */
    return_stack& operator<<(cl_object o)
    {
        env->values[env->nvalues++] = o;
        return *this;
    }
    /** A function to finally return all the values that were grouped. */
    cl_object return_value()
    {
        return env->values[0];
    }

private:
    /* Copy constructors are hidden and forbidden. */
    return_stack(const return_stack& s);
    const return_stack& operator=(const return_stack& s);
    const cl_env_ptr env;
};

template <class Ret, class T1>
cl_object wrap(Ret F(T1 a1), cl_object a)
{

    auto eclObject = nth_arg(a, 1);
    auto b1 = toCpp<T1>(eclObject);
    return toEcl(F(b1));
}

template <typename Ret, typename... Args, std::size_t... Index>
cl_object wrapImpl(
    Ret func(Args...), std::index_sequence<Index...>, cl_object frame)
{
    std::tuple<Args...> t;
    ((std::get<Index>(t) =
             toCpp<typename std::tuple_element<Index, decltype(t)>::type>(
                 nth_arg(frame, Index))),
        ...);
    return toEcl(std::apply(func, t));
}

template <typename Ret, typename... Args>
cl_object wrap(Ret func(Args...), cl_object frame)
{
    return wrapImpl(func, std::index_sequence_for<Args...>{}, frame);
}

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
