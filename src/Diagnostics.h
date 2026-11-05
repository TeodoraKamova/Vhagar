#pragma once
#include "Locator.h"
#include <iostream>
#include <sstream>
// highly inspired by the diagnostics.h from the impl_ideas :)

namespace diagnostics
{
    [[noreturn]] void dragon(const std::string& errstr);

    /// <summary>
    /// not for external use
    /// Base case of variadic template function for noting errors.
    /// /// </summary>
    template <typename T> [[noreturn]] void errorf_impl(const T& msg)
    {
        std::stringstream errss;
        errss << msg;
        dragon(errss.str());
    }

    /// <summary>
    /// not for external use
    /// Recursive case of variadic template function for noting errors.
    /// </summary>
    template <typename T, typename... Args>
    [[noreturn]] void errorf_impl(const T& msg, Args&&... args)
    {
        // std::cerr << msg;
        // errorf_impl(std::forward<Args>(args)...);
        std::stringstream errss;
        errss << msg << ' ';
        (errss << ... << args);
        dragon(errss.str());
    }

    /// <summary>
    /// Print an error message that is the concatenation of all template
    /// arguments without a location. Then, terminate execution with the
    /// appropriate error code. Only use this for errors that are not tied to a
    /// source location, like invalid command line arguments.
    /// </summary>
    template <typename... Args> [[noreturn]] void error(Args&&... args)
    {
        std::cerr << "error: ";
        errorf_impl(std::forward<Args>(args)...);
    }

    /// <summary>
    /// Print an todo-error message that is the concatenation of all template
    /// arguments without a location. Then, terminate execution with the
    /// appropriate error code.
    /// </summary>
    template <typename... Args> [[noreturn]] void todo(Args&&... args)
    {
        std::cerr << "TODO: ";
        errorf_impl(std::forward<Args>(args)...);
    }

    /// Print an error message that is the concatenation of all template arguments
    /// with a prefix indicating the source position of the given locatable.
    /// Then, terminate execution with the appropriate error code.
    /// This function should be used for most error messages.
    ///
    /// Example use:
    /// if (...) {
    ///     errorloc(loc, "invalid construct: ", x);
    /// }
    template <typename... Args> [[noreturn]] void errorloc(const Location& loc, Args&&... args)
    {
        std::cerr << loc << ": error: ";
        errorf_impl(std::forward<Args>(args)...);
    }
} // namespace diagnostics
