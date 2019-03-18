#pragma once
#include <ecl/ecl.h>
#include <cstdint>
#include <iostream>
#include <functional>
#include <boost/callable_traits/args.hpp>
#include <type_traits>
#include "pack.hpp"

namespace eclpp
{

// Helper macros
#define ECL_DECLTYPE_RETURN(...)                                               \
    decltype(__VA_ARGS__)                                                      \
    {                                                                          \
        return __VA_ARGS__;                                                    \
    }

#define ECL_DECLARE_NUMERIC_TYPE(cpp_name__, ecl_name__)                       \
    template <>                                                                \
    struct convert<cpp_name__>                                                 \
    {                                                                          \
        static cpp_name__ to_cpp(cl_object v)                                  \
        {                                                                      \
            return ecl_to_##ecl_name__(v);                                     \
        }                                                                      \
        static cl_object to_ecl(cpp_name__ v)                                  \
        {                                                                      \
            return ecl_make_##ecl_name__(v);                                   \
        }                                                                      \
    };

// Helper struct definition to convert numerical types between ecl and
// cpp.
template <typename T, typename Enable = void>
struct convert;

template <typename T>
struct convert<T*>
{
    static T* to_cpp(cl_object v)
    {
        return reinterpret_cast<T*>(ecl_to_pointer(v));
    }

    static cl_object to_ecl(T* v)
    {
        return ecl_make_pointer(v);
    }
};
// Helper struct definition to convert wrapper between ecl and cpp.
template <typename T>
struct convert_wrapper_type
{
    static T to_cpp(cl_object v)
    {
        return T{v};
    }
    static cl_object to_ecl(T v)
    {
        return v.get();
    }
};

// Helper function to convert ecl's double float to double. This is
// needed because we want to tuse the `ECL_DECLARE_NUMERIC_TYPE` macro
// to generate the conversion struct. Ecl is missing a function with
// name ecl_to_double_float, therefore we define it our self. This
// lets us use the macro.
double ecl_to_double_float(cl_object v)
{
    return ecl_to_double(v);
}

// See above.
double ecl_to_single_float(cl_object v)
{
    return ecl_to_float(v);
}

cl_object ecl_make_base_string(std::string str)
{
    return ecl_make_simple_base_string(str.c_str(), str.size());
}

std::string ecl_to_base_string(cl_object obj)
{
    cl_index index, length = obj->string.fillp;

    std::string tmp(length, ' ');
    for (index = 0; index < length; index++)
    {
        ecl_character c = obj->string.self[index];
        if (!ECL_BASE_CHAR_CODE_P(c))
            FEerror("Cannot coerce string ~A to a base-string", 1, obj);
        tmp[index] = c;
    }
    return tmp;
}

cl_object ecl_make_constant_string(const char* str)
{
    return ecl_make_constant_base_string(str, -1);
}

char* ecl_to_constant_string(cl_object v)
{
    return ecl_base_string_pointer_safe(v);
}
// Define conversion between ecl and cpp for numerical types
ECL_DECLARE_NUMERIC_TYPE(std::int8_t, int8_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint8_t, uint8_t);
ECL_DECLARE_NUMERIC_TYPE(std::int16_t, int16_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint16_t, uint16_t);
ECL_DECLARE_NUMERIC_TYPE(std::int32_t, int32_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint32_t, uint32_t);
ECL_DECLARE_NUMERIC_TYPE(std::int64_t, int64_t);
ECL_DECLARE_NUMERIC_TYPE(std::uint64_t, uint64_t);
ECL_DECLARE_NUMERIC_TYPE(float, single_float);
ECL_DECLARE_NUMERIC_TYPE(double, double_float);

// Register string type
ECL_DECLARE_NUMERIC_TYPE(std::string, base_string);
ECL_DECLARE_NUMERIC_TYPE(const char*, constant_string);

// Helper function to convert from cpp to ecl.
template <typename T>
auto to_ecl(T&& v) -> ECL_DECLTYPE_RETURN(
    convert<std::decay_t<T>>::to_ecl(std::forward<T>(v)));

// Helper function to convert from ecl to cpp
template <typename T>
auto to_cpp(cl_object v)
    -> ECL_DECLTYPE_RETURN(convert<std::decay_t<T>>::to_cpp(v));

struct wrapper
{
    wrapper() = default;

    wrapper(cl_object handle)
        : m_handle(handle)
    {
    }

    cl_object get() const
    {
        return m_handle;
    }

    operator cl_object() const
    {
        return m_handle;
    }

    bool operator==(wrapper other)
    {
        return m_handle == other.m_handle;
    }

    bool operator!=(wrapper other)
    {
        return m_handle != other.m_handle;
    }

    cl_object m_handle = ECL_NIL;
};

struct val : wrapper
{
    using base_t = wrapper;

    using base_t::base_t;

    template <typename T, typename = std::enable_if_t<(
                              !std::is_same<std::decay_t<T>, val>{}
                              && !std::is_same<std::decay_t<T>, cl_object>{})>>
    val(T&& x)
        : base_t(to_ecl(std::forward<T>(x)))
    {
    }

    template <typename T, typename = std::enable_if_t<std::is_same<T,
                              decltype(to_cpp<T>(cl_object{}))>{}>>
    operator T() const
    {
        return to_cpp<T>(m_handle);
    }
    template <typename T, typename = std::enable_if_t<std::is_same<T&,
                              decltype(to_cpp<T>(cl_object{}))>{}>>
    operator T&() const
    {
        return to_cpp<T>(m_handle);
    }
};

template <>
struct convert<val> : convert_wrapper_type<val>
{
};

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
val make_foreign(Args&&... args)
{
    using finalizer = finalizer_storage<T>;
    val foreign = ecl_make_foreign_data(
        ECL_NIL, sizeof(T), new T(std::forward<Args>(args)...));
    if (finalizer::finalizer == ECL_NIL)
    {
        finalizer::finalizer =
            ecl_make_cfun(reinterpret_cast<cl_objectfn_fixed>(finalize<T>),
                ecl_read_from_cstring("finalizer"), ECL_NIL, 1);
    }
    si_set_finalizer(foreign, finalizer::finalizer);
    return foreign;
}

template <typename T>
T& get_foreign(val v)
{
    return *static_cast<T*>(ecl_foreign_data_pointer_safe(v));
}

template <typename T>
struct convert_foreign_type
{
    template <typename U>
    static cl_object to_ecl(U&& v)
    {
        return make_foreign<T>(std::forward<U>(v));
    } // namespace eclpp
    static T& to_cpp(cl_object v)
    {
        return get_foreign<T>(v);
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
    : convert_foreign_type<T>
{
};

// Conversion from enums to underlaying type
template <typename T>
struct convert<T, std::enable_if_t<std::is_enum<T>::value>>
{
    static cl_object to_ecl(T v)
    {
        return convert<std::underlying_type_t<T>>::to_ecl(
            static_cast<std::underlying_type_t<T>>(v));
    }

    static T to_cpp(cl_object a)
    {
        std::cout << "To cpp" << std::endl;
        return static_cast<T>(convert<std::underlying_type_t<T>>::to_cpp(a));
    }
};

template <typename R, typename Fn>
auto define_function_helper_impl(Fn fn, pack<R>, pack<>)
{
    static const Fn fn_ = fn;
    return []() -> cl_object { return to_ecl(std::invoke(fn_)); };
}

template <typename Fn, typename R, typename T1>
auto define_function_helper_impl(Fn fn, pack<R>, pack<T1>)
{
    static const Fn fn_ = fn;
    return [](cl_object a1) -> cl_object {
        return to_ecl(std::invoke(fn_, to_cpp<T1>(a1)));
    };
}
template <typename Fn, typename R, typename T1, typename T2>
auto define_function_helper_impl(Fn fn, pack<R>, pack<T1, T2>)
{
    static const Fn fn_ = fn;
    return [](cl_object a1, cl_object a2) -> cl_object {
        return to_ecl(std::invoke(fn_, to_cpp<T1>(a1), to_cpp<T2>(a2)));
    };
}
template <typename Fn, typename R, typename T1, typename T2, typename T3>
auto define_function_helper_impl(Fn fn, pack<R>, pack<T1, T2, T3>)
{
    static const Fn fn_ = fn;
    return [](cl_object a1, cl_object a2, cl_object a3) -> cl_object {
        return to_ecl(
            std::invoke(fn_, to_cpp<T1>(a1), to_cpp<T2>(a2), to_cpp<T3>(a3)));
    };
}
template <typename Fn, typename T1>
auto define_function_helper_impl(Fn fn, pack<void>, pack<T1>)
{
    static const Fn fn_ = fn;
    return [](cl_object a1) -> cl_object {
        std::invoke(fn_, to_cpp<T1>(a1));
        return ECL_NIL;
    };
}

template <typename Fn, typename T1, typename T2>
auto define_function_helper_impl(Fn fn, pack<void>, pack<T1, T2>)
{
    static const Fn fn_ = fn;
    return [](cl_object a1, cl_object a2) -> cl_object {
        std::invoke(fn_, to_cpp<T1>(a1), to_cpp<T2>(a2));
        return ECL_NIL;
    };
}

template <typename Fn, typename T1, typename T2, typename T3>
auto define_function_helper_impl(Fn fn, pack<void>, pack<T1, T2, T3>)
{
    static const Fn fn_ = fn;
    return [](cl_object a1, cl_object a2, cl_object a3) -> cl_object {
        std::invoke(fn_, to_cpp<T1>(a1), to_cpp<T2>(a2), to_cpp<T3>(a3));
        return ECL_NIL;
    };
}

template <typename Fn>
auto define_function_helper_impl(Fn fn, pack<void>, pack<>)
{
    static const Fn fn_ = fn;
    return []() -> cl_object {
        std::invoke(fn_);
        return ECL_NIL;
    };
}

template <typename Fn, typename... Args>
auto define_function_helper(Fn fn, pack<Args...>)
{
    return define_function_helper_impl(
        fn, pack<std::result_of_t<Fn(Args...)>>{}, pack<Args...>{});
}

template <typename Fn>
static void define_function(const std::string& name, Fn fn)
{
    using args_t = function_args_t<Fn>;
    constexpr auto args_size = pack_size_v<args_t>;

    auto func = (cl_objectfn_fixed) + define_function_helper(fn, args_t{});
    ecl_def_c_function(ecl_read_from_cstring(name.c_str()), func, args_size);
}

struct move_sequence
{
    move_sequence() = default;
    move_sequence(const move_sequence&) = delete;
    move_sequence(move_sequence&& other)
    {
        other.moved_from_ = true;
    };

    bool moved_from_ = false;
};
// See
// https://stackoverflow.com/questions/8610571/what-is-rvalue-reference-for-this
// why ref qulifer on member functions are needed.  For ecl we
// actually don't need the move_sequence, because foreign types are
// not registered during destruction.
template <typename Tag, typename T, int Seq = 0>
struct type_definer : move_sequence
{
    using this_t = type_definer;
    using next_t = type_definer<Tag, T, Seq + 1>;

    std::string m_type_name = {};

    type_definer(type_definer&&) = default;

    type_definer(std::string type_name)
        : m_type_name(std::move(type_name))
    {
    }

    ~type_definer()
    {
        if (!moved_from_)
        {
            // We don't need this because foreign type must not be
            // registered before using under ecl. For guile this may
            // be needed.
        }
    }

    template <int Seq2, typename Enable = std::enable_if_t<Seq2 + 1 == Seq>>
    type_definer(type_definer<Tag, T, Seq2> r)
        : move_sequence{std::move(r)}
        , m_type_name{std::move(r.m_type_name)}
    {
    }

    /**
     * Define a Scheme procedure `([type-name])` that returns a Scheme
     * value holding a default constructed `T` instance.
     */
    next_t constructor() &&
    {
        define_function(m_type_name, &make_foreign<T>);
        return {std::move(*this)};
    }

    /**
     * Define a Scheme procedure `([type-name] ...)` that returns a
     * Scheme value holding the result of invoking `fn(args...)`.
     */
    template <typename Fn>
    next_t constructor(Fn fn) &&
    {
        define_function(m_type_name, fn);
        return {std::move(*this)};
    }

    /**
     * Define a Scheme procedure `(make-[type-name])` that returns a
     * Scheme value holding a default constructed `T` instance.
     */
    next_t maker() &&
    {
        define_function("make-" + m_type_name, &make_foreign<T>);
        return {std::move(*this)};
    }

    /**
     * Define a Scheme procedure `(make-[type-name] ...)` that returns
     * the result of invoking `fn(...)`.
     */
    template <typename Fn>
    next_t maker(Fn fn) &&
    {
        define_function("make-" + m_type_name, fn);
        return {std::move(*this)};
    }

    /**
     * Define a Scheme procedure `([type-name]-[name] ...)` that returns
     * the result of invoking `fn(...)`.
     */
    template <typename Fn>
    next_t define(std::string name, Fn fn) &&
    {
        define_function(m_type_name + "-" + name, fn);
        return {std::move(*this)};
    }
};
template <typename T, typename Tag = T>
type_definer<Tag, T> type(std::string type_name)
{
    return {type_name};
}

template <typename Tag, int Seq = 0>
struct definer
{
    using this_t = definer;
    using next_t = definer<Tag, Seq + 1>;

    definer() = default;
    definer(definer&&) = default;

    template <int Seq2, typename Enable = std::enable_if_t<Seq2 + 1 == Seq>>
    definer(definer<Tag, Seq2>)
    {
    }

    /**
     * Defines a Scheme procedure `(name ...)` that returns the result
     * of invoking `fn(...)`.
     */
    template <typename Fn>
    next_t define(std::string name, Fn fn) &&
    {
        define_function(name, fn);
        return {std::move(*this)};
    }
};

template <typename Tag, int Seq = 0>
struct group_definer
{
    using this_t = group_definer;
    using next_t = group_definer<Tag, Seq + 1>;

    std::string group_name_ = {};

    group_definer(std::string name)
        : group_name_{std::move(name)}
    {
    }

    group_definer(group_definer&&) = default;

    template <int Seq2, typename Enable = std::enable_if_t<Seq2 + 1 == Seq>>
    group_definer(group_definer<Tag, Seq2> other)
        : group_name_{std::move(other.group_name_)}
    {
    }

    /**
     * Defines a Scheme procedure `([group-name]-name ...)` that returns
     * the result of invoking `fn(...)`.
     */
    template <typename Fn>
    next_t define(std::string name, Fn fn) &&
    {
        define_function(group_name_ + "-" + name, fn);
        return {std::move(*this)};
    }
};

/**
 * Returns a `scm::detail::definer` that can be used to add definitions
 * to the current module.
 */
template <typename Tag = void>
definer<Tag> group()
{
    return {};
}

/**
 * Returns a named `scm::detail::group_definer` that can be used to add
 * definitions to the current module.
 */
template <typename Tag = void>
group_definer<Tag> group(std::string group_name)
{
    return {std::move(group_name)};
}

struct list : wrapper
{
    using base_t = wrapper;
    using base_t::base_t;

    using iterator = list;
    using value_type = val;

    list()
        : base_t{ECL_NIL} {};
    list end() const
    {
        return {};
    }
    list begin() const
    {
        return *this;
    }

    explicit operator bool()
    {
        return m_handle != ECL_NIL;
    }

    val operator*() const
    {
        return val{cl_car(m_handle)};
    }

    list& operator++()
    {
        m_handle = cl_cdr(m_handle);
        return *this;
    }

    list operator++(int)
    {
        auto result = *this;
        result.m_handle = cl_cdr(m_handle);
        return result;
    }
};

template <>
struct convert<list> : convert_wrapper_type<list>
{
};

} // Namespace eclpp
