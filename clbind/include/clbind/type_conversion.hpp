#pragma once

#include <unordered_map>
#include <string>
#include <typeindex>
#include <ecl/ecl.h>
#include <functional>

namespace clbind
{
class class_registry
{
public:
    void add(std::type_index index, const std::string& name)
    {
        m_reg.insert({index, name});
    }

    std::string name(std::type_index index)
    {
        if (auto iter = m_reg.find(index); iter != m_reg.end())
        {
            return iter->second;
        }
        else
        {
            throw std::runtime_error("Class not found");
        }
    }

private:
    std::unordered_map<std::type_index, std::string> m_reg;
};

template <typename T>
struct unused_type
{
};

template <typename T1, typename T2>
struct define_if_different
{
    using type = T1;
};

template <typename T>
struct define_if_different<T, T>
{
    using type = unused_type<T>;
};

template <typename T1, typename T2>
using define_if_different_t = typename define_if_different<T1, T2>::type;

// Mapping between c+++ and lisp types
template <typename T>
struct static_type_mapping;

template <>
struct static_type_mapping<char>
{
    using type = char;
    static std::string lisp_type()
    {
        return ":char";
    }
};

template <>
struct static_type_mapping<define_if_different_t<unsigned char, uint8_t>>
{
    using type = unsigned char;
    static std::string lisp_type()
    {
        return ":unsigned-char";
    }
};

template <>
struct static_type_mapping<define_if_different_t<int, int32_t>>
{
    using type = int;
    static std::string lisp_type()
    {
        return ":int";
    }
};

template <>
struct static_type_mapping<uint8_t>
{
    using type = uint8_t;
    static std::string lisp_type()
    {
        return ":uint8-t";
    }
};

template <>
struct static_type_mapping<int8_t>
{
    using type = int8_t;
    static std::string lisp_type()
    {
        return ":int8-t";
    }
};

template <>
struct static_type_mapping<uint16_t>
{
    using type = uint16_t;
    static std::string lisp_type()
    {
        return ":uint16-t";
    }
};

template <>
struct static_type_mapping<int16_t>
{
    using type = int16_t;
    static std::string lisp_type()
    {
        return ":int16-t";
    }
};

template <>
struct static_type_mapping<uint32_t>
{
    using type = uint32_t;
    static std::string lisp_type()
    {
        return ":uint32-t";
    }
};

template <>
struct static_type_mapping<int32_t>
{
    using type = int32_t;
    static std::string lisp_type()
    {
        return ":int32-t";
    }
};

template <>
struct static_type_mapping<uint64_t>
{
    using type = uint64_t;
    static std::string lisp_type()
    {
        return ":uint64-t";
    }
};

template <>
struct static_type_mapping<int64_t>
{
    using type = int64_t;
    static std::string lisp_type()
    {
        return ":int64-t";
    }
};

template <>
struct static_type_mapping<float>
{
    using type = float;
    static std::string lisp_type()
    {
        return ":float";
    }
};

template <>
struct static_type_mapping<double>
{
    using type = double;
    static std::string lisp_type()
    {
        return ":double";
    }
};

template <typename T>
using static_type_mapping_t = typename static_type_mapping<T>::type;

template <typename CppT, typename Enable = void>
struct convert_to_lisp
{
    template <typename LispT>
    LispT* operator()(CppT&&)
    {
        static_assert(sizeof(CppT) == 0,
            "No appropriate specialization for convert_to_lisp");
        return nullptr;
    }
};

template <typename CppT>
struct convert_to_lisp<CppT,
    typename std::enable_if<std::is_fundamental<CppT>::value>::type>
{
    CppT operator()(CppT cpp) const
    {
        return cpp;
    }
};

template <typename T>
using convert_to_lisp_t = convert_to_lisp<T>;

template <typename T>
auto convert_to_lisp_helper(T&& cpp_val)
    -> decltype(convert_to_lisp<T>()(std::forward<T>> (cpp_val)))
{
    return convert_to_lisp<T>()(std::forward<T>(cpp_val));
}

template <class... Ts>
using all_of = std::conjunction<Ts...>;
template <class... Ts>
using any_of = std::disjunction<Ts...>;
template <class... Ts>
using none_of = std::negation<any_of<Ts...>>;

template <class T, template <class> class... Predicates>
using satisfies_all_of = all_of<Predicates<T>...>;
template <class T, template <class> class... Predicates>
using satisfies_any_of = any_of<Predicates<T>...>;
template <class T, template <class> class... Predicates>
using satisfies_none_of = none_of<Predicates<T>...>;

/// Strip the class from a method type
template <typename T>
struct remove_class
{
};
template <typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...)>
{
    typedef R type(A...);
};
template <typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...) const>
{
    typedef R type(A...);
};

template <typename F>
struct strip_function_object
{
    using type = typename remove_class<decltype(&F::operator())>::type;
};

// Extracts the function signature from a function, function pointer or lambda.
template <typename Function, typename F = std::remove_reference_t<Function>>
using function_signature_t = std::conditional_t<std::is_function<F>::value, F,
    typename std::conditional_t<std::is_pointer<F>::value
                                    || std::is_member_pointer<F>::value,
        std::remove_pointer<F>, strip_function_object<F>>::type>;

/// Returns true if the type looks like a lambda: that is, isn't a function,
/// pointer or member pointer.  Note that this can catch all sorts of other
/// things, too; this is intended to be used in a place where passing a lambda
/// makes sense.
template <typename T>
using is_lambda = satisfies_none_of<std::remove_reference_t<T>,
    std::is_function, std::is_pointer, std::is_member_pointer>;

class cpp_function
{
public:
    cpp_function()
    {
    }
    cpp_function(std::nullptr_t)
    {
        std::cout << "nullptr" << std::endl;
    }

    /// Construct a cpp_function from a vanilla function pointer
    template <typename Return, typename... Args>
    cpp_function(Return (*f)(Args...))
    {
        std::cout << "cpp_function(Return (*f)(Args...))" << std::endl;
        initialize(f, f);
    }

    /// Construct a cpp_function from a lambda function (possibly with internal
    /// state)
    template <typename Func,
        typename = std::enable_if_t<is_lambda<Func>::value>>
    cpp_function(Func&& f)
    {
        std::cout << "cpp_function(Func&& f))" << std::endl;
        initialize(std::forward<Func>(f), (function_signature_t<Func>*)nullptr);
    }

    /// Construct a cpp_function from a class method (non-const)
    template <typename Return, typename Class, typename... Arg>
    cpp_function(Return (Class::*f)(Arg...))
    {
        std::cout << "cpp_function(Return (Class::*f)(Arg...))" << std::endl;
        initialize(
            [f](Class* c, Arg... args) -> Return { return (c->*f)(args...); },
            (Return(*)(Class*, Arg...)) nullptr);
    }

    /// Construct a cpp_function from a class method (const)
    template <typename Return, typename Class, typename... Arg>
    cpp_function(Return (Class::*f)(Arg...) const)
    {
        std::cout << "cpp_function(Return (Class::*f)(Arg...) const)"
                  << std::endl;
        initialize([f](const Class* c,
                       Arg... args) -> Return { return (c->*f)(args...); },
            (Return(*)(const Class*, Arg...)) nullptr);
    }

    template <typename Func, typename Return, typename... Args>

    void initialize(Func&& f, Return (*)(Args...))
    {
        std::cout << "Initialize" << std::endl;
        struct capture
        {
            std::remove_reference_t<Func> f;
        };

        std::cout << sizeof(capture) << std::endl;
    }
};
// template <typename R, typename... Args>
// struct ReturnTypeAdapter
// {
//     using return_type =
//     decltype(static_type_mapping(std::declval<R>())::type);

//     return_type operator()(
//         const void* functor, static_type_mapping_t<Args>... args)
//     {
//         auto std_func =
//             reinterpret_cast<const std::function<R(Args...)>*>(functor);
//     };
// };

// template <typename R, typename... Args>
// struct CallFunctor
// {
//     using return_type
// }
template <typename T>
struct finalizer_storage
{
    static cl_object finalizer;
};

template <typename T>
cl_object finalize(cl_object obj)
{
    T* data = static_cast<T*>(ecl_foreign_data_pointer_safe(obj));
    delete data;
    return ECL_T;
}

template <typename T>
cl_object finalizer_storage<T>::finalizer = ECL_NIL;

template <typename T, typename... Args>
cl_object make_foreign(Args&&... args)
{
    using finalizer = finalizer_storage<T>;
    cl_object foreign = ecl_make_foreign_data(
        ECL_NIL, sizeof(T), new T(std::forward<Args>(args)...));
    // if (finalizer::finalizer == ECL_NIL)
    // {
    //     finalizer::finalizer =
    //         ecl_make_cfun(reinterpret_cast<cl_objectfn_fixed>(finalize<T>),
    //             ecl_read_from_cstring("finalizer"), ECL_NIL, 1);
    // }
    // si_set_finalizer(foreign, finalizer::finalizer);
    return foreign;
}

template <typename T>
T& get_foreign(cl_object v)
{
    return *static_cast<T*>(ecl_foreign_data_pointer_safe(v));
}

template <typename T, typename Enable = void>
struct convert;

template <>
struct convert<int32_t>
{
    static int32_t to_cpp(cl_object v)
    {
        std::cout << "int32_t" << std::endl;
        return ecl_to_int32_t(v);
    }
    static cl_object to_ecl(int32_t v)
    {
        std::cout << "int32_t" << std::endl;
        return ecl_make_int32_t(v);
    }
};

template <typename T>
struct convert<T*>
{
    static T* to_cpp(cl_object v)
    {
        std::cout << "static T* to_cpp(cl_object v)" << std::endl;
        return reinterpret_cast<T*>(ecl_to_pointer(v));
    }

    static cl_object to_ecl(T* v)
    {
        std::cout << "static cl_object to_ecl(T* v)" << std::endl;
        return ecl_make_pointer(v);
    }
};

// Assume that every other type is foreign
template <typename T>
struct convert<T, std::enable_if_t<!std::is_fundamental<T>::value
                                   && !std::is_enum<T>::value &&
                                   // only value types are supported at
                                   // the moment but the story might
                                   // change later...
                                   !std::is_pointer<T>::value>>
{
    template <typename U>
    static cl_object to_ecl(U&& v)
    {
        std::cout << "cl_object to_ecl(U&& v)" << std::endl;
        return make_foreign<T>(std::forward<U>(v));
    }
    static T& to_cpp(cl_object v)
    {
        std::cout << "T& to_cpp(cl_object v)" << std::endl;
        return get_foreign<T>(v);
    }
};

template <typename T>
auto to_ecl(T&& v)
    -> decltype(convert<std::decay_t<T>>::to_ecl(std::forward<T>(v)))
{
    return convert<std::decay_t<T>>::to_ecl(std::forward<T>(v));
}

// Helper function to convert from ecl to cpp
template <typename T>
auto to_cpp(cl_object v) -> decltype(convert<std::decay_t<T>>::to_cpp(v))
{
    return convert<std::decay_t<T>>::to_cpp(v);
}

template <typename R, typename... Args>
struct ReturnTypeAdapter
{
    using return_type = R;
    return_type operator()(const void* functor, Args... args)
    {
        auto std_func =
            reinterpret_cast<const std::function<R(Args...)>*>(functor);

        return ((*std_func)(args...));
    }
};

template <typename... Args>
struct ReturnTypeAdapter<void, Args...>
{
    void operator()(const void* functor, Args... args)
    {
        auto std_func =
            reinterpret_cast<const std::function<void(Args...)>*>(functor);
        ((*std_func)(args...));
    }
};

template <typename R, typename... Args>
struct CallFunctor
{
    using return_type = decltype(ReturnTypeAdapter<R, Args...>()(
        std::declval<const void*>(), std::declval<Args>()...));

    static return_type apply(const void* functor, Args... args)
    {
        return ReturnTypeAdapter<R, Args...>()(functor, args...);
    }
};

template <typename R, typename... Args>
class FunctionWrapper
{
public:
    typedef std::function<R(Args...)> functor_t;

    explicit FunctionWrapper(const functor_t& f)
    {
        p_function = f;
    }

    const void* ptr()
    {
        return reinterpret_cast<const void*>(&p_function);
    }

private:
    functor_t p_function;
};

template <typename R, typename... Args>
auto createFunctor(std::function<R(Args...)> f)
{
    return reinterpret_cast<const void*>(CallFunctor<R, Args...>::apply);
}

} // namespace clbind
