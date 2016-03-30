/*
 * The MIT License (MIT)
 *
 * Copyright © 2015-2016 Franklin "Snaipe" Mathieu <http://snai.pe/>
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
#ifndef MIMICK_PREPROCESS_H_
# define MIMICK_PREPROCESS_H_

# define MMK_NOOP do {} while(0)
# define MMK_NOOP_FN(...) MMK_NOOP

# define MMK_EXPAND(x) x
# define MMK_IDENTITY(...) __VA_ARGS__

# define MMK_STR(x) MMK_EXPAND(MMK_STR_(x))
# define MMK_STR_(x) #x

# define MMK_VA_TAIL(...) MMK_EXPAND(MMK_VA_TAIL_HELPER(MMK_VA_TAIL_SELECT(__VA_ARGS__), __VA_ARGS__))

# define MMK_VA_TAIL_HELPER(N, ...)  MMK_EXPAND(MMK_VA_TAIL_HELPER_(N, __VA_ARGS__))
# define MMK_VA_TAIL_HELPER_(N, ...) MMK_EXPAND(MMK_VA_TAIL_HELPER_##N(__VA_ARGS__))
# define MMK_VA_TAIL_HELPER_1(Head)
# define MMK_VA_TAIL_HELPER_2(Head, ...) __VA_ARGS__

# define MMK_VA_HEAD(...) MMK_EXPAND(MMK_VA_HEAD_HELPER(MMK_VA_TAIL_SELECT(__VA_ARGS__), __VA_ARGS__))

# define MMK_VA_HEAD_HELPER(N, ...)  MMK_EXPAND(MMK_VA_HEAD_HELPER_(N, __VA_ARGS__))
# define MMK_VA_HEAD_HELPER_(N, ...) MMK_EXPAND(MMK_VA_HEAD_HELPER_##N(__VA_ARGS__))
# define MMK_VA_HEAD_HELPER_1(Head) Head
# define MMK_VA_HEAD_HELPER_2(Head, ...) Head

# define MMK_VA_TAIL_SELECT(...) MMK_EXPAND(MMK_VA_TAIL_SELECT64(__VA_ARGS__, \
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, \
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, \
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, \
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, \
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, \
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, \
        2, 2, 1, _))

# define MMK_VA_NARGS(...) MMK_EXPAND(MMK_VA_TAIL_SELECT64(__VA_ARGS__, \
        63, 62, 61, 60,                         \
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
        49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
        29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

# define MMK_VA_TAIL_SELECT64( \
        _01, _02, _03, _04, _05, _06, _07, _08, _09, _10, \
        _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
        _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
        _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
        _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
        _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
        _61, _62, _63, X, ...) X

# define MMK_APPLY_1(Macro, ...)
# define MMK_APPLY_2(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_1(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_3(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_2(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_4(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_3(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_5(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_4(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_6(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_5(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_7(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_6(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_8(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_7(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_9(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_8(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_10(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_9(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_11(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_10(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_12(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_11(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_13(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_12(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_14(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_13(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_15(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_14(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_16(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_15(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_17(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_16(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_18(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_17(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_19(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_18(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_20(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_19(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_21(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_20(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_22(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_21(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_23(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_22(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_24(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_23(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_25(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_24(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_26(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_25(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_27(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_26(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_28(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_27(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_29(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_28(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_30(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_29(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_31(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_30(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_32(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_31(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_33(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_32(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_34(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_33(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_35(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_34(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_36(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_35(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_37(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_36(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_38(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_37(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_39(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_38(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_40(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_39(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_41(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_40(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_42(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_41(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_43(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_42(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_44(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_43(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_45(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_44(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_46(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_45(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_47(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_46(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_48(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_47(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_49(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_48(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_50(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_49(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_51(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_50(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_52(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_51(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_53(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_52(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_54(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_53(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_55(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_54(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_56(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_55(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_57(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_56(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_58(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_57(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_59(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_58(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_60(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_59(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_61(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_60(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_62(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_61(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_63(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_62(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_64(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_63(Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_65(Macro, Prefix, Head, ...) Macro(Prefix, Head) MMK_EXPAND(MMK_APPLY_64(Macro, Prefix, __VA_ARGS__))

# define MMK_APPLY__(Macro, Prefix, n, ...) MMK_EXPAND(MMK_APPLY_##n(Macro, Prefix, __VA_ARGS__,))
# define MMK_APPLY_(Macro, n, Prefix, ...) MMK_EXPAND(MMK_APPLY__(Macro, Prefix, n, __VA_ARGS__))
# define MMK_APPLY(Macro, ...) MMK_EXPAND(MMK_APPLY_(Macro, MMK_VA_NARGS(__VA_ARGS__), __VA_ARGS__))

# define MMK_PAIR_APPLY_1(Macro, Prefix, ...)
# define MMK_PAIR_APPLY_2(Macro, Prefix, ...)
# define MMK_PAIR_APPLY_3(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_1(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_5(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_3(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_7(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_5(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_9(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_7(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_11(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_9(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_13(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_11(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_15(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_13(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_17(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_15(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_19(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_17(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_21(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_19(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_23(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_21(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_25(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_23(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_27(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_25(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_29(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_27(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_31(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_29(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_33(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_31(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_35(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_33(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_37(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_35(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_39(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_37(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_41(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_39(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_43(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_41(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_45(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_43(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_47(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_45(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_49(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_47(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_51(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_49(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_53(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_51(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_55(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_53(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_57(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_55(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_59(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_57(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_61(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_59(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_63(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_61(Macro, Prefix, __VA_ARGS__))
# define MMK_PAIR_APPLY_65(Macro, Prefix, First, Second, ...) Macro(Prefix, First, Second) MMK_EXPAND(MMK_PAIR_APPLY_63(Macro, Prefix, __VA_ARGS__))

# define MMK_PAIR_APPLY__(Macro, Prefix, n, ...) MMK_EXPAND(MMK_PAIR_APPLY_##n(Macro, Prefix, __VA_ARGS__,))
# define MMK_PAIR_APPLY_(Macro, n, Prefix, ...) MMK_EXPAND(MMK_PAIR_APPLY__(Macro, Prefix, n, __VA_ARGS__))
# define MMK_PAIR_APPLY(Macro, ...) MMK_EXPAND(MMK_PAIR_APPLY_(Macro, MMK_VA_NARGS(__VA_ARGS__), __VA_ARGS__))

# define MMK_PARAM_LIST_1(...) void
# define MMK_PARAM_LIST_2(T, X, ...) T X
# define MMK_PARAM_LIST_4(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_2(__VA_ARGS__))
# define MMK_PARAM_LIST_6(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_4(__VA_ARGS__))
# define MMK_PARAM_LIST_8(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_6(__VA_ARGS__))
# define MMK_PARAM_LIST_10(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_8(__VA_ARGS__))
# define MMK_PARAM_LIST_12(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_10(__VA_ARGS__))
# define MMK_PARAM_LIST_14(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_12(__VA_ARGS__))
# define MMK_PARAM_LIST_16(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_14(__VA_ARGS__))
# define MMK_PARAM_LIST_18(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_16(__VA_ARGS__))
# define MMK_PARAM_LIST_20(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_18(__VA_ARGS__))
# define MMK_PARAM_LIST_22(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_20(__VA_ARGS__))
# define MMK_PARAM_LIST_24(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_22(__VA_ARGS__))
# define MMK_PARAM_LIST_26(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_24(__VA_ARGS__))
# define MMK_PARAM_LIST_28(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_26(__VA_ARGS__))
# define MMK_PARAM_LIST_30(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_28(__VA_ARGS__))
# define MMK_PARAM_LIST_32(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_30(__VA_ARGS__))
# define MMK_PARAM_LIST_34(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_32(__VA_ARGS__))
# define MMK_PARAM_LIST_36(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_34(__VA_ARGS__))
# define MMK_PARAM_LIST_38(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_36(__VA_ARGS__))
# define MMK_PARAM_LIST_40(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_38(__VA_ARGS__))
# define MMK_PARAM_LIST_42(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_40(__VA_ARGS__))
# define MMK_PARAM_LIST_44(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_42(__VA_ARGS__))
# define MMK_PARAM_LIST_46(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_44(__VA_ARGS__))
# define MMK_PARAM_LIST_48(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_46(__VA_ARGS__))
# define MMK_PARAM_LIST_50(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_48(__VA_ARGS__))
# define MMK_PARAM_LIST_52(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_50(__VA_ARGS__))
# define MMK_PARAM_LIST_54(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_52(__VA_ARGS__))
# define MMK_PARAM_LIST_56(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_54(__VA_ARGS__))
# define MMK_PARAM_LIST_58(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_56(__VA_ARGS__))
# define MMK_PARAM_LIST_60(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_58(__VA_ARGS__))
# define MMK_PARAM_LIST_62(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_60(__VA_ARGS__))
# define MMK_PARAM_LIST_64(T, X, ...) T X, MMK_EXPAND(MMK_PARAM_LIST_62(__VA_ARGS__))

# define MMK_PARAM_LIST__(n, ...) MMK_EXPAND(MMK_PARAM_LIST_##n(__VA_ARGS__,))
# define MMK_PARAM_LIST_(n, ...) MMK_EXPAND(MMK_PARAM_LIST__(n, __VA_ARGS__))
# define MMK_PARAM_LIST(...) MMK_EXPAND(MMK_PARAM_LIST_(MMK_VA_NARGS(__VA_ARGS__), __VA_ARGS__))

#endif /* !MIMICK_PREPROCESS_H_ */
