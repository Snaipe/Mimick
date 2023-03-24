#include <mimick.h>
#include <mimick/assert.h>
#include <stdlib.h>
#include <limits.h>

#include "libfoo.h"

mmk_mock_define (fn_ii_mock, int, int);
mmk_mock_define (fn_vv_mock, void);

static int MMK_MANGLE(fn_ii_va_mock, serialize_va)(int i, va_list vl,
    size_t *sz_out, struct mmk_va_param ***va_out)
{
    if (i <= 0)
        return 0;

    struct mmk_va_param **va_data = mmk_malloc(i * sizeof (void *));

    // Expect all va parameters to be int
    for (int j = 0; j < i; ++j) {
        mmk_make_va_param(va_data[j], vl, int);
    }

    *sz_out = i;
    *va_out = va_data;

    return 1;
}

mmk_mock_define (fn_ii_va_mock, int, int, mmk_va_args);

static int valid;

void set_valid(void) {
    valid = 1;
}

static int int_eq_called;
static int expected_int;

int int_eq(int val) {
    int_eq_called = 1;
    return val == expected_int;
}

mmk_mock_define (fn_dd_mock, double, double);

double mul_two(double d)
{
	return d * 2;
}

mmk_mock_define (fn_ff_mock, float, float);
float mmk_f_abs(float f)
{
	return f < 0 ? -f : f;
}

#define check_called_exact(Expr, Times) do {                                   \
        size_t times = (Times);                                                \
        if (times == 0) {                                                      \
            mmk_assert(mmk_verify(Expr, .never = 1));                          \
        } else {                                                               \
            mmk_assert(mmk_verify(Expr, .times = times));                      \
            mmk_assert(mmk_verify(Expr, .at_least = times));                   \
            mmk_assert(mmk_verify(Expr, .at_most = times));                    \
            mmk_assert(mmk_verify(Expr, .at_least = times, .at_most = times)); \
            mmk_assert(!mmk_verify(Expr, .never = 1));                         \
        }                                                                      \
        if (times > 1) {                                                       \
            mmk_assert(!mmk_verify(Expr, .times = times - 1));                 \
            mmk_assert(mmk_verify(Expr, .at_least = times - 1));               \
            mmk_assert(!mmk_verify(Expr, .at_most = times - 1));               \
        }                                                                      \
        if (times < (size_t)-1) {                                              \
            mmk_assert(!mmk_verify(Expr, .times = times + 1));                 \
            mmk_assert(mmk_verify(Expr, .at_most = times + 1));                \
            mmk_assert(!mmk_verify(Expr, .at_least = times + 1));              \
        }                                                                      \
    } while (0)

#define mmk_abs(x) ((x) < 0 ? -(x) : (x))

int main(void)
{
    mmk_mock("fn_vv", fn_vv_mock);
    mmk_when(fn_vv(), .then_call = (mmk_fn) set_valid);

    check_called_exact(fn_vv(), 0);
    fn_vv();
    mmk_assert(valid);
    check_called_exact(fn_vv(), 1);
    mmk_reset(fn_vv);

    int err = 0xfefefefe;
    mmk_mock("fn_ii", fn_ii_mock);
    mmk_when(fn_ii(1), .then_return = mmk_val(int, 1), .then_errno = err);
    mmk_when(fn_ii(mmk_eq(int, 2)), .then_return = mmk_val(int, 2), .then_errno = err + 1);
    mmk_when(fn_ii(mmk_ge(int, 3)), .then_return = mmk_val(int, 3), .then_errno = err + 2);
    mmk_when(fn_ii(mmk_gt(int, 4)), .then_return = mmk_val(int, 4), .then_errno = err + 3);
    mmk_when(fn_ii(mmk_le(int, -2)), .then_return = mmk_val(int, 5), .then_errno = err + 4);
    mmk_when(fn_ii(mmk_lt(int, -3)), .then_return = mmk_val(int, 6), .then_errno = err + 5);
    mmk_when(fn_ii(mmk_that(int, int_eq)), .then_return = mmk_val(int, 7), .then_errno = err + 6);

    mmk_assert(fn_ii(1) == 1 && errno == err);
    mmk_assert(fn_ii(2) == 2 && errno == err + 1);
    mmk_assert(fn_ii(3) == 3 && errno == err + 2);
    mmk_assert(fn_ii(4) == 3 && errno == err + 2);
    mmk_assert(fn_ii(5) == 4 && errno == err + 3);
    mmk_assert(fn_ii(6) == 4 && errno == err + 3);
    mmk_assert(fn_ii(INT_MAX)  == 4 && errno == err + 3);
    mmk_assert(fn_ii(-1) == 0);
    mmk_assert(fn_ii(-2) == 5 && errno == err + 4);
    mmk_assert(fn_ii(-3) == 5 && errno == err + 4);
    mmk_assert(fn_ii(-4) == 6 && errno == err + 5);
    mmk_assert(fn_ii(-5) == 6 && errno == err + 5);
    mmk_assert(fn_ii(INT_MIN) == 6 && errno == err + 5);

    expected_int = 1;
    mmk_assert(fn_ii(1) == 7 && errno == err + 6 && int_eq_called);
    int_eq_called = 0;

    check_called_exact(fn_ii(mmk_any(int)), 14);
    check_called_exact(fn_ii(1), 2);
    check_called_exact(fn_ii(2), 1);
    check_called_exact(fn_ii(3), 1);
    check_called_exact(fn_ii(4), 1);
    check_called_exact(fn_ii(5), 1);
    check_called_exact(fn_ii(6), 1);
    check_called_exact(fn_ii(INT_MAX), 1);
    check_called_exact(fn_ii(-1), 1);
    check_called_exact(fn_ii(-2), 1);
    check_called_exact(fn_ii(-3), 1);
    check_called_exact(fn_ii(-4), 1);
    check_called_exact(fn_ii(-5), 1);
    check_called_exact(fn_ii(INT_MIN), 1);
    check_called_exact(fn_ii(12), 0);
    mmk_reset(fn_ii);


    mmk_mock("fn_ii_va", fn_ii_va_mock);
    mmk_when(fn_ii_va(1, 42), .then_return = mmk_val(int, 1));

    mmk_assert(fn_ii_va(1, 42) == 1);

    mmk_reset(fn_ii_va);

    mmk_mock("fn_dd", fn_dd_mock);
    mmk_when(fn_dd(mmk_any(double)), .then_call = (mmk_fn) mul_two);
    mmk_assert(mmk_abs(fn_dd(2.) - 4.) < 1e-4);
    mmk_assert(mmk_abs(fn_dd(10.) - 20.) < 1e-4);
    mmk_reset(fn_dd);

    mmk_mock("fn_ff", fn_ff_mock);
    mmk_when(fn_ff(mmk_any(float)), .then_call = (mmk_fn) mmk_f_abs);
    mmk_assert(mmk_abs(1.) == fn_ff(1.));
    mmk_assert(mmk_abs(-42.) == fn_ff(-42.));
    mmk_assert(mmk_abs(-21.) == fn_ff(21.));
    mmk_assert(mmk_abs(4.) == fn_ff(-4.));
    mmk_reset(fn_ff);

    return 0;
}
