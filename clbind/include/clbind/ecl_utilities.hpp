#pragma once

#include <stdexcept>
#include <sstream>
#include <ecl/ecl.h>

namespace clbind
{

cl_object nth_arg(cl_object arglist, int i)
{
    if (i >= arglist->frame.size)
    {
        std::stringstream ss;
        ss << "Missing argument #" << i << " in wrapped function.";
        throw std::runtime_error(ss.str());
    }
    return ((cl_object*)arglist->frame.base)[i];
}

struct return_stack
{
    return_stack()
        : env(ecl_process_env())
    {
        env->nvalues = 0;
        env->values[0] = ECL_NIL;
    }

    return_stack& operator<<(cl_object o)
    {
        env->values[env->nvalues++] = o;
        return *this;
    }

    cl_object return_value()
    {
        return env->values[0];
    }

private:
    return_stack(const return_stack& s);
    const return_stack& operator=(const return_stack& s);
    const cl_env_ptr env;
};

} // namespace clbind
