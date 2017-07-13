/*
 * The MIT License (MIT)
 *
 * Copyright © 2016 Franklin "Snaipe" Mathieu <http://snai.pe/>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MIMICK_H_
# define MIMICK_H_

# include <stddef.h>
# include "mimick/preprocess.h"

struct mmk_va_param {
    size_t size;
    char data[];
};

typedef void(*mmk_fn)(void);
typedef struct mmk_stub *mmk_stub;

/**
 * An invalid stub. Used for error checking if creating a stub fails.
 */
# define MMK_STUB_INVALID ((mmk_stub)0)

/**
 * An invalid mock. Used for error checking if creating a mock fails.
 */
# define MMK_MOCK_INVALID ((mmk_fn)0)

/* Stub API */

/**
 * @defgroup stub_api Stub API
 * @{
 */

/**
 * Gets the current stub object from within a stub function.
 *
 * It is undefined behaviour to call mmk_ctx outside of a stub function.
 *
 * @returns The current stub object.
 */
mmk_stub mmk_ctx(void);

/**
 * Get the user context from a stub object.
 *
 * The passed stub object must be a valid stub object returned by
 * mmk_stub_create that has not yet been destroyed by mmk_stub_destroy.
 *
 * NULL is returned if MMK_STUB_INVALID is passed.
 *
 * @param[in] stub  A stub object.
 *
 * @returns The user context or NULL
 */
void *mmk_stub_context(mmk_stub stub);

/**
 * Creates a stub object and hijacks calls to the function targeted by
 * the specified symbol target string to the specified function.
 *
 * It is undefined behaviour if the prototype of the passed function pointer
 * does not match the targeted function, or if the passed function pointer
 * does not reference a valid C function.
 *
 * If the function cannot be located in the specified target, mmk_stub_create
 * fails and returns MMK_STUB_INVALID.
 *
 * @see symbol_target_string
 *
 * @param[in] target    The symbol target string.
 * @param[in] fn        The replacing function.
 * @param[in] ctx       The user context -- can be anything you want.
 *                      May be NULL.
 *
 * @returns The created stub object or MMK_STUB_INVALID.
 */
mmk_stub mmk_stub_create(const char *target, mmk_fn fn, void *ctx);

/**
 * Destroy a stub object and restores the original function it targets.
 *
 * The passed stub object must be a valid stub object returned by
 * mmk_stub_create, or MMK_STUB_INVALID.
 *
 * It is undefined behaviour if mmk_stub_destroy is called on a stub object
 * that has already been destroyed previously.
 *
 * @param[in] stub  The stub object to destroy.
 */
void mmk_stub_destroy(mmk_stub stub);

/** @} */

/* Mock API */

/**
 * @defgroup mock_api Mock API
 * @{
 */

/**
 * Convenience wrapper to get an addressable temporary object from a lvalue.
 */
# define mmk_val(Type, ...) (&(Type) { __VA_ARGS__ })

/**
 * Defines a mock blueprint and typedef the Id to a function pointer type
 * matching the mock prototype.
 *
 * @param[in] Id            The Id of this blueprint.
 * @param[in] ReturnType    The return type of the mock prototype.
 * @param[in] ...           The list of parameter types of the mock prototype.
 *                          Leave empty if the function takes no parameters.
 */
# define mmk_mock_define(Id, ReturnType, ...) <internal>

/**
 * Instanciates a function mock from a symbol target string and a blueprint ID.
 *
 * The blueprint must have been previously defined with mmk_mock_define.
 *
 * It is undefined behaviour if the prototype of the specified blueprint
 * does not match the prototype of the target function.
 *
 * When the instanciation succeeds, the macro returns a function pointer
 * representing the newly created mock object. This function pointer can
 * be used identically as the targeted function.
 *
 * If the instanciation fails, the macro either aborts the process or,
 * if the noabort option is set, returns MMK_MOCK_INVALID.
 *
 * The macro optionally takes a designated initializer list for setting options
 * during the mock creation. Valid options are:
 *
 *  * .noabort = 1: Return MMK_MOCK_INVALID if the mock creation fails
 *                  instead of aborting the process.
 *
 * @see mmk_stub_create
 * @see symbol_target_string
 *
 * @param[in] Target    The symbol target string.
 * @param[in] Id        The Id of a matching mock blueprint.
 * @param[in] ...       Optional mock options as a designated initializer list.
 *                      e.g: `.noabort = 1`.
 *
 * @returns The mock object or MMK_MOCK_INVALID.
 */
# define mmk_mock(Target, Id, ...) <internal>

/**
 * Defines a contract for a defined set of parameters.
 *
 * The macro takes a expression that calls exactly once a mocked function
 * with either values or matchers.
 *
 * If a matcher is specified, all parameters must be matchers.
 *
 * The macro takes after the calling expression one or more designated
 * initializer `.key = value` pairs to specify the expected behaviour
 * when a call matches the parameters set in the calling expression.
 *
 * Valid keys are:
 *
 *  * `.then_return = <ptr>`: return the dereferenced value of `<ptr>`.
 *
 *  * `.then_errno = <err>`: set errno to the `<err>` integer value.
 *
 *  * `.then_call = <fnptr>`: calls the function pointed at by `<fnptr>`
 *    and returns the value the function itself returns.
 *
 *    `.then_return` overrides the returned value, but the function still
 *    gets executed.
 *
 *    The function pointer **must** match the prototype of the mock function.
 *
 * @param[in] CallExpr  The calling expression
 * @param[in] ...       The expected behaviour key/value pairs
 */
# define mmk_when(CallExpr, ...) <internal>

/**
 * Verifies the interactions of a mocked function call matching a set of
 * parameters.
 *
 * The macro takes a expression that calls exactly once a mocked function
 * with either values or matchers.
 *
 * If a matcher is specified, all parameters must be matchers.
 *
 * The macro takes after the calling expression one or more designated
 * initializer `.key = value` pairs to verify how many times a function
 * was called with matching parameters.
 *
 * Valid keys are:
 *
 *  * `.times = <n>`: Was the function called exactly `<n>` times?
 *  * `.at_least = <n>`: Was the function called at least `<n>` times?
 *    (can be specified together with `.at_most`)
 *  * `.at_most = <n>`: Was the function called at most `<n>` times?
 *    (can be specified together with `.at_least`)
 *  * `.never = 1`: Was the function never called?
 *  * `.matching = <fnptr>`: Is the number of times the function was called
 *    meeting the requirements of `<fnptr>`?
 *
 *    `<fnptr>` is a function pointer of type `int (*)(size_t)` that takes
 *    the number of time a function has been called with matching parameters,
 *    and returns either non-zero if the verification passes or zero otherwise.
 *
 * @param[in] CallExpr  The calling expression
 * @param[in] ...       The verification key/value pairs
 *
 * @returns non-zero if the verification passes, zero otherwise.
 */
# define mmk_verify(CallExpr, ...) <internal>

/**
 * Destroys the mock object and restores the original 'unmocked' function.
 *
 * The function can take either a mock object returned by `mmk_mock` (including
 * MMK_MOCK_INVALID), or a mocked function.
 *
 * If MMK_MOCK_INVALID is passed, the function returns and nothing happens.
 *
 * It is undefined behaviour to pass a function that isn't mocked.
 *
 * @param[in] Fn    The mock object or the mocked function
 */
# define mmk_reset(Fn) <internal>

/**
 *  Placeholder type for variadic function parameters for use in
 *  mmk_mock_define.
 */
# define mmk_va_args <internal>

/** @} */

/* Matcher API */

/**
 * Match any value of type `Type`.
 *
 * @param[in] Type The type of the value to match.
 */
# define mmk_any(Type) <internal>

/**
 * Match any value equal to `Val`.
 *
 * @param[in] Type  The type of the value to match.
 * @param[in] Val   The reference to compare values with.
 */
# define mmk_eq(Type, Val) <internal>

/**
 * Match any value not equal to `Val`.
 *
 * @param[in] Type  The type of the value to match.
 * @param[in] Val   The reference to compare values with.
 */
# define mmk_ne(Type, Val) <internal>

/**
 * Match any value strictly lower than `Val`.
 *
 * @param[in] Type  The type of the value to match.
 * @param[in] Val   The reference to compare values with.
 */
# define mmk_lt(Type, Val) <internal>

/**
 * Match any value lower or equal to `Val`.
 *
 * @param[in] Type  The type of the value to match.
 * @param[in] Val   The reference to compare values with.
 */
# define mmk_le(Type, Val) <internal>

/**
 * Match any value strictly greater than `Val`.
 *
 * @param[in] Type  The type of the value to match.
 * @param[in] Val   The reference to compare values with.
 */
# define mmk_gt(Type, Val) <internal>

/**
 * Match any value greater or equal to `Val`.
 *
 * @param[in] Type  The type of the value to match.
 * @param[in] Val   The reference to compare values with.
 */
# define mmk_ge(Type, Val) <internal>

/**
 * Match any value accepted by a predicate.
 *
 * The predicate must have the prototype `int predicate(<T>)`, where
 * `<T>` is the type of the non-variadic parameter to match.
 *
 * If the parameter is in a variadic parameter pack, the prototype of
 * the predicate becomes `int predicate(struct mmk_va_param *)`.
 *
 * The predicate shall return zero if the parameter does not match
 * its rules, and non-zero otherwise.
 *
 * @param[in] Predicate A function pointer to a predicate.
 */
# define mmk_that(Predicate) <internal>

/* Documentation concepts */

/**
 * @defgroup symbol_target_string Symbol target strings
 * @ingroup general_concepts
 * @{
 *
 * Symbol target strings are strings of the form `"symbol_name(@scope)?"`,
 * i.e. a valid C identifier followed by an optional scope.
 *
 * A scope specifies the module where the function is mocked. Valid values
 * are either:
 *
 * 1. `self` which always refers to the main executable. Not specifying a
 *    scope is equivalent to using the `self` scope.
 *
 *    Regardless of the scope, the function is always mocked in the main
 *    executable, `self` only specifies that it shall only be mocked there.
 *
 * 2. Values of the form `selector:value`, where `selector` is one of `file`,
 *    `lib`, or `sym`.
 *
 *    1. If the `file` selector is specified, then `value` is interpreted as a
 *       full path and the scope is the loaded module at this specific path.
 *
 *    2. If the `lib` selector is specified, then `value` is interpreted as a
 *       name, and the scope is the loaded module with this name.
 *
 *       Note: This resolves differently between platforms and is the
 *          platform-independent way of specifying a library:
 *
 *          - On Linux, this resolves to a loaded library named `lib<value>.so`.
 *          - On Windows, this finds a loaded library named `<value>.dll`.
 *          - On Darwin, this finds a loaded library named `lib<value>.dylib`.
 *
 *    3. If the `sym` selector is specified, then `value` is interpreted as a
 *       symbol name, and the scope is the first loaded module defining this
 *       symbol.
 *
 * Examples:
 *
 *    * "foo": Mock the `foo` function in the main executable only.
 *
 *    * "foo@self": Same as `foo`
 *
 *    * "foo@file:/lib/libbar.so": Mock the `foo` function in `/lib/libbbar.so`.
 *
 *    * "foo@lib:bar": Mock the `foo` function in the `bar` library.
 *
 *    * "foo@sym:bar": Mock the `foo` function in the first loaded library that
 *      defines `bar`.
 *
 * @}
 */

/* Internals */

# include "mimick/mock.h"
# include "mimick/matcher.h"

# ifndef MMK_DO_NOT_UNOPTIMIZE
#  if defined __GNUC__ && !defined __clang__
#   pragma GCC optimize "O0"
#  elif defined _MSC_VER
#   pragma optimize("", off)
#  endif
# endif

# if defined __clang__
#  define mmk_no_optimize __attribute__((optnone))
# elif __GNUC__ >= 4 && __GNUC_MINOR__ >= 4
#  define mmk_no_optimize __attribute__((optimize(0)))
# else
#  define mmk_no_optimize
# endif

# ifndef MMK_DO_NOT_DISABLE_WARNINGS
#  if defined __GNUC__
#   pragma GCC diagnostic ignored "-Wunused-result"
#  elif defined _MSC_VER
#   pragma warning(disable : 6031)
#  endif
# endif

#endif /* !MIMICK_H_ */
