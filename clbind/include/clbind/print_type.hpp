#pragma once

namespace clbind
{
template <typename T>
void print_type(T&&)
{
    static_assert(std::is_same<T, int>::value && !std::is_same<T, int>::value,
        "Compilation failed because you wanted to read the type. See below");
}
} // namespace clbind
