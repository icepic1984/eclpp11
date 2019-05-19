#pragma once

#include <unordered_map>
#include <string>
#include <typeindex>

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

} // namespace clbind
