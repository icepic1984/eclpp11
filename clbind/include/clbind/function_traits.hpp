// as seen on http://functionalcpp.wordpress.com/2013/08/05/function-traits/
namespace clbind
{

template <class T>
struct dependent_false : std::false_type
{
};

template <class F>
struct function_traits;

template <class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t size = sizeof...(Args);

    using args = std::tuple<Args...>;
};

template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)>
{
};

template <class R, class... Args>
struct function_traits<R(Args...) const&&> : public function_traits<R(Args...)>
{
};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...)>
    : public function_traits<R(C&, Args...)>
{
};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) const>
    : public function_traits<R(const C&, Args...)>
{
};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) &&>
    : public function_traits<R(C&&, Args...)>
{
};

template <class C, class R, class... Args>
struct function_traits<R (C::*const&)(Args...)>
    : public function_traits<R(C&, Args...)>
{
};

template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{
};

template <typename F>
struct function_traits : public function_traits<decltype(&F::operator())>
{
};

template <class F>
struct function_traits<F&> : public function_traits<F>
{
};

template <class F>
struct function_traits<F&&> : public function_traits<F>
{
};

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
