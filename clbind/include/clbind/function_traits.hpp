#pragma once
#include <tuple>
#include <functional>

// as seen on http://functionalcpp.wordpress.com/2013/08/05/function-traits/
namespace clbind
{

template <typename T>
struct remove_first_type
{
};

template <typename T, typename... Ts>
struct remove_first_type<std::tuple<T, Ts...>>
{
    using type = std::tuple<Ts...>;
};

template <typename T>
using remove_first_type_t = typename remove_first_type<T>::type;

template <typename T>
struct dependent_false : std::false_type
{
};

template <typename F>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t size = sizeof...(Args);

    using args = std::tuple<Args...>;
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)>
{
};

template <typename R, typename... Args>
struct function_traits<R(Args...) const&&> : public function_traits<R(Args...)>
{
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)>
    : public function_traits<R(C&, Args...)>
{
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const>
    : public function_traits<R(const C&, Args...)>
{
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) &&>
    : public function_traits<R(C&&, Args...)>
{
};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*const&)(Args...)>
    : public function_traits<R(C&, Args...)>
{
};

template <typename C, typename R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{
};

template <typename F>
struct function_traits
{
private:
    using call_type = function_traits<decltype(&F::operator())>;

public:
    using return_type = typename call_type::return_type;

    static constexpr std::size_t size = call_type::size - 1;

    using args = remove_first_type_t<typename call_type::args>;
};

template <typename F>
struct function_traits<F&> : public function_traits<F>
{
};

template <typename F>
struct function_traits<F&&> : public function_traits<F>
{
};

template <typename F>
using function_args_t = typename function_traits<F>::args;

template <typename F>
using function_return_type_t = typename function_traits<F>::return_type;

template <typename R, typename Args>
struct as_function;

template <typename R, typename... Args>
struct as_function<R, std::tuple<Args...>>
{
    using type = std::function<R(Args...)>;
};

template <typename R, typename... Args>
using as_function_t = typename as_function<R, Args...>::type;

// Unit tests
namespace detail
{

// function template
static_assert(
    std::is_same_v<clbind::function_traits<void(float, char, int)>::args,
        std::tuple<float, char, int>>,
    "template function argument type mismatch");

static_assert(
    std::is_same_v<clbind::function_traits<void()>::args, std::tuple<>>,
    "template function argument type mismatch");

static_assert(
    std::is_same_v<clbind::function_traits<void(float, char, int)>::return_type,
        void>,
    "template function return type mismatch");

// function pointer
static_assert(
    std::is_same_v<clbind::function_traits<void (*)(float, char, int)>::args,
        std::tuple<float, char, int>>,
    "function pointer argument type mismatch");

static_assert(
    std::is_same_v<clbind::function_traits<void (*)()>::args, std::tuple<>>,
    "function pointer argument type mismatch");

static_assert(
    std::is_same_v<
        clbind::function_traits<void (*)(float, char, int)>::return_type, void>,
    "template function return type mismatch");

// function reference
static_assert(
    std::is_same_v<clbind::function_traits<void (&)(float, char, int)>::args,
        std::tuple<float, char, int>>,
    "function reference argument type mismatch");

static_assert(
    std::is_same_v<clbind::function_traits<void (&)()>::args, std::tuple<>>,
    "function reference argument type mismatch");

static_assert(
    std::is_same_v<
        clbind::function_traits<void (&)(float, char, int)>::return_type, void>,
    "function reference return type mismatch");

// ref qualifier
static_assert(std::is_same_v<
                  clbind::function_traits<void(float, char, int) const&&>::args,
                  std::tuple<float, char, int>>,
    "function reference argument type mismatch");

static_assert(
    std::is_same_v<clbind::function_traits<void() const&&>::args, std::tuple<>>,
    "function reference argument type mismatch");

static_assert(
    std::is_same_v<
        clbind::function_traits<void(float, char, int) const&&>::return_type,
        void>,
    "function reference return type mismatch");

struct foo
{
};

// member function pointer
static_assert(
    std::is_same_v<
        clbind::function_traits<void (foo::*)(float, char, int)>::args,
        std::tuple<foo&, float, char, int>>,
    "member function pointer argument type mismatch");

static_assert(
    std::is_same_v<
        clbind::function_traits<void (foo::*const&)(float, char, int)>::args,
        std::tuple<foo&, float, char, int>>,
    "member function pointer argument type mismatch");

static_assert(
    std::is_same_v<
        clbind::function_traits<void (foo::*)(float, char, int) const>::args,
        std::tuple<const foo&, float, char, int>>,
    "member function pointer argument type mismatch");

static_assert(
    std::is_same_v<
        clbind::function_traits<void (foo::*)(float, char, int) &&>::args,
        std::tuple<foo&&, float, char, int>>,
    "member function pointer argument type mismatch");

} // namespace detail
} // namespace clbind
