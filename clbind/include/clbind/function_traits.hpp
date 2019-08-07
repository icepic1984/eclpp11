// as seen on http://functionalcpp.wordpress.com/2013/08/05/function-traits/
namespace clbind
{

template <class T>
struct dependent_false : std::false_type
{
};

template <class F>
struct function_traits;

// function pointer
template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)>
{
    // static_assert(dependent_false<R (*)(Args...)>(), "function pointer");
};

template <class R, class... Args>
struct function_traits<R(Args...)>
{
    // static_assert(dependent_false<R(Args...)>(), "function template");
    using return_type = R;

    static constexpr std::size_t size = sizeof...(Args);

    using args = std::tuple<Args...>;
};

// member function pointer
template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...)>
    : public function_traits<R(C&, Args...)>
{
    // static_assert(
    //     dependent_false<R (C::*)(Args...)>(), "member function pointer");
};

// const member function pointer
template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) const>
    : public function_traits<R(C&, Args...)>
{
    // static_assert(dependent_false<R (C::*)(Args...) const>(),
    //     "const member function pointer");
};

// member object pointer
template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{
    // static_assert(dependent_false<R(C::*)>(), "member object pointer");
};

template <typename F>
struct function_traits : public function_traits<decltype(&F::operator())>
{
    // static_assert(dependent_false<F>(), "operator");
};
// functor
// template <class F>
// struct function_traits
// {
//     // static_assert(dependent_false<F>(), "operator");

// private:
//     using call_type = function_traits<decltype(&F::operator())>;

// public:
//     using return_type = typename call_type::return_type;

//     static constexpr std::size_t arity = call_type::arity - 1;

//     template <std::size_t N>
//     struct argument
//     {
//         static_assert(N < arity, "error: invalid parameter index.");
//         using type = typename call_type::template argument<N + 1>::type;
//     };
// };

template <class F>
struct function_traits<F&> : public function_traits<F>
{
    //  static_assert(dependent_false<F>(), "F& function template");
};

template <class F>
struct function_traits<F&&> : public function_traits<F>
{
    //   static_assert(dependent_false<F>(), "F&& function template");
};
} // namespace clbind
