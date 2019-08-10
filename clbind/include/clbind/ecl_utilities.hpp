#pragma once

#include <stdexcept>
#include <sstream>
#include <ecl/ecl.h>
#include <ecl/internal.h>

namespace clbind
{

using callback_t = cl_object (*)(void* f, cl_object);

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

cl_object symbol(const char* package, const char* name)
{
    cl_object p = ecl_find_package(package);
    return _ecl_intern(name, p);
}

cl_object symbol(const char* name)
{
    cl_object output = ecl_read_from_cstring_safe(name, ECL_NIL);
    if (output == ECL_NIL || type_of(output) != t_symbol)
    {
        std::stringstream ss;
        ss << "The string \"" << name
           << "\" does not name a valid common lisp symbol.";
        throw std::runtime_error(ss.str());
    }
    return output;
}

static cl_object callback_function(cl_narg narg, ...)
{
    {
        cl_env_ptr the_env = ecl_process_env();
        cl_object output;
        ECL_STACK_FRAME_VARARGS_BEGIN(narg, narg, frame);
        {
            cl_object closure = the_env->function;
            cl_object closure_env = closure->cclosure.env;
            {
                cl_object env = closure_env;
                callback_t wrapper = (callback_t)ECL_CONS_CAR(closure_env);
                void* f = (void*)ECL_CONS_CDR(closure_env);
                output = wrapper(f, frame);
            }
        }
        ECL_STACK_FRAME_VARARGS_END(frame);
        return output;
    }
}

void define_function(cl_object symbol, callback_t callback, void* f)
{
    cl_object env = ecl_cons((cl_object)callback, (cl_object)f);
    cl_object fn = ecl_make_cclosure_va(callback_function, env, ECL_NIL);
    si_fset(2, symbol, fn);
    cl_export(1, symbol);
}

void define_function(
    const char* package, const char* name, callback_t callback, void* f)
{
    define_function(symbol(package, name), callback, f);
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
