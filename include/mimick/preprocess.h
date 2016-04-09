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

/* Welcome to the magical exhibition of sanity loss!
 *
 * We shall see what contortions the human mind must sometimes go through to
 * get a nice and usable API in the C programming language.
 *
 * Put on your seatbelts because this is going to be a wild ride.
 */

# define MMK_NOOP do {} while(0)
# define MMK_NOOP_FN(...) MMK_NOOP

# define MMK_SWALLOW(...)
# define MMK_NOPAREN(x) x
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

# define MMK_APPLY_N1(N, Macro, ...)
# define MMK_APPLY_N2(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N1 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N3(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N2 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N4(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N3 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N5(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N4 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N6(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N5 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N7(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N6 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N8(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N7 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N9(N, Macro, Prefix, Head, ...)  Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N8 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N10(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N9 (MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N11(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N10(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N12(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N11(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N13(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N12(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N14(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N13(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N15(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N14(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N16(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N15(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N17(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N16(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N18(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N17(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N19(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N18(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N20(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N19(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N21(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N20(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N22(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N21(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N23(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N22(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N24(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N23(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N25(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N24(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N26(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N25(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N27(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N26(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N28(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N27(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N29(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N28(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N30(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N29(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N31(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N30(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N32(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N31(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N33(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N32(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N34(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N33(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N35(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N34(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N36(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N35(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N37(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N36(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N38(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N37(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N39(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N38(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N40(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N39(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N41(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N40(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N42(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N41(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N43(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N42(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N44(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N43(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N45(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N44(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N46(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N45(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N47(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N46(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N48(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N47(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N49(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N48(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N50(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N49(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N51(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N50(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N52(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N51(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N53(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N52(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N54(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N53(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N55(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N54(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N56(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N55(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N57(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N56(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N58(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N57(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N59(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N58(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N60(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N59(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N61(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N60(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N62(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N61(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N63(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N62(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N64(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N63(MMK_INC(N), Macro, Prefix, __VA_ARGS__))
# define MMK_APPLY_N65(N, Macro, Prefix, Head, ...) Macro(N, Prefix, Head) MMK_EXPAND(MMK_APPLY_N64(MMK_INC(N), Macro, Prefix, __VA_ARGS__))

# define MMK_APPLY_N__(Macro, Prefix, n, ...) MMK_EXPAND(MMK_APPLY_N##n(0, Macro, Prefix, __VA_ARGS__,))
# define MMK_APPLY_N_(Macro, n, Prefix, ...) MMK_EXPAND(MMK_APPLY_N__(Macro, Prefix, n, __VA_ARGS__))
# define MMK_APPLY_N_NE(Macro, Prefix, ...) MMK_EXPAND(MMK_APPLY_N_(Macro, MMK_VA_NARGS(,__VA_ARGS__), Prefix, __VA_ARGS__))

# define MMK_APPLY_N_SELECT(Macro, Prefix, ...) MMK_EXPAND(MMK_APPLY_N_SELECT_HELPER(MMK_VA_TAIL_SELECT(__VA_ARGS__), Macro, Prefix, __VA_ARGS__))

# define MMK_APPLY_N_SELECT_HELPER(N, ...)  MMK_EXPAND(MMK_APPLY_N_SELECT_HELPER_(N, __VA_ARGS__))
# define MMK_APPLY_N_SELECT_HELPER_(N, ...) MMK_EXPAND(MMK_APPLY_N_SELECT_HELPER_##N(__VA_ARGS__))
# define MMK_APPLY_N_SELECT_HELPER_1(...)
# define MMK_APPLY_N_SELECT_HELPER_2(Macro, Prefix, ...) MMK_EXPAND(MMK_APPLY_N_NE(Macro, Prefix, MMK_VA_TAIL(__VA_ARGS__)))

# define MMK_APPLY_N(...) MMK_EXPAND(MMK_APPLY_N_SELECT(__VA_ARGS__))

# define MMK_LAST_1(...)
# define MMK_LAST_2(Head, ...)  Head
# define MMK_LAST_3(Head, ...)  MMK_EXPAND(MMK_LAST_2 (__VA_ARGS__))
# define MMK_LAST_4(Head, ...)  MMK_EXPAND(MMK_LAST_3 (__VA_ARGS__))
# define MMK_LAST_5(Head, ...)  MMK_EXPAND(MMK_LAST_4 (__VA_ARGS__))
# define MMK_LAST_6(Head, ...)  MMK_EXPAND(MMK_LAST_5 (__VA_ARGS__))
# define MMK_LAST_7(Head, ...)  MMK_EXPAND(MMK_LAST_6 (__VA_ARGS__))
# define MMK_LAST_8(Head, ...)  MMK_EXPAND(MMK_LAST_7 (__VA_ARGS__))
# define MMK_LAST_9(Head, ...)  MMK_EXPAND(MMK_LAST_8 (__VA_ARGS__))
# define MMK_LAST_10(Head, ...) MMK_EXPAND(MMK_LAST_9 (__VA_ARGS__))
# define MMK_LAST_11(Head, ...) MMK_EXPAND(MMK_LAST_10(__VA_ARGS__))
# define MMK_LAST_12(Head, ...) MMK_EXPAND(MMK_LAST_11(__VA_ARGS__))
# define MMK_LAST_13(Head, ...) MMK_EXPAND(MMK_LAST_12(__VA_ARGS__))
# define MMK_LAST_14(Head, ...) MMK_EXPAND(MMK_LAST_13(__VA_ARGS__))
# define MMK_LAST_15(Head, ...) MMK_EXPAND(MMK_LAST_14(__VA_ARGS__))
# define MMK_LAST_16(Head, ...) MMK_EXPAND(MMK_LAST_15(__VA_ARGS__))
# define MMK_LAST_17(Head, ...) MMK_EXPAND(MMK_LAST_16(__VA_ARGS__))
# define MMK_LAST_18(Head, ...) MMK_EXPAND(MMK_LAST_17(__VA_ARGS__))
# define MMK_LAST_19(Head, ...) MMK_EXPAND(MMK_LAST_18(__VA_ARGS__))
# define MMK_LAST_20(Head, ...) MMK_EXPAND(MMK_LAST_19(__VA_ARGS__))
# define MMK_LAST_21(Head, ...) MMK_EXPAND(MMK_LAST_20(__VA_ARGS__))
# define MMK_LAST_22(Head, ...) MMK_EXPAND(MMK_LAST_21(__VA_ARGS__))
# define MMK_LAST_23(Head, ...) MMK_EXPAND(MMK_LAST_22(__VA_ARGS__))
# define MMK_LAST_24(Head, ...) MMK_EXPAND(MMK_LAST_23(__VA_ARGS__))
# define MMK_LAST_25(Head, ...) MMK_EXPAND(MMK_LAST_24(__VA_ARGS__))
# define MMK_LAST_26(Head, ...) MMK_EXPAND(MMK_LAST_25(__VA_ARGS__))
# define MMK_LAST_27(Head, ...) MMK_EXPAND(MMK_LAST_26(__VA_ARGS__))
# define MMK_LAST_28(Head, ...) MMK_EXPAND(MMK_LAST_27(__VA_ARGS__))
# define MMK_LAST_29(Head, ...) MMK_EXPAND(MMK_LAST_28(__VA_ARGS__))
# define MMK_LAST_30(Head, ...) MMK_EXPAND(MMK_LAST_29(__VA_ARGS__))
# define MMK_LAST_31(Head, ...) MMK_EXPAND(MMK_LAST_30(__VA_ARGS__))
# define MMK_LAST_32(Head, ...) MMK_EXPAND(MMK_LAST_31(__VA_ARGS__))
# define MMK_LAST_33(Head, ...) MMK_EXPAND(MMK_LAST_32(__VA_ARGS__))
# define MMK_LAST_34(Head, ...) MMK_EXPAND(MMK_LAST_33(__VA_ARGS__))
# define MMK_LAST_35(Head, ...) MMK_EXPAND(MMK_LAST_34(__VA_ARGS__))
# define MMK_LAST_36(Head, ...) MMK_EXPAND(MMK_LAST_35(__VA_ARGS__))
# define MMK_LAST_37(Head, ...) MMK_EXPAND(MMK_LAST_36(__VA_ARGS__))
# define MMK_LAST_38(Head, ...) MMK_EXPAND(MMK_LAST_37(__VA_ARGS__))
# define MMK_LAST_39(Head, ...) MMK_EXPAND(MMK_LAST_38(__VA_ARGS__))
# define MMK_LAST_40(Head, ...) MMK_EXPAND(MMK_LAST_39(__VA_ARGS__))
# define MMK_LAST_41(Head, ...) MMK_EXPAND(MMK_LAST_40(__VA_ARGS__))
# define MMK_LAST_42(Head, ...) MMK_EXPAND(MMK_LAST_41(__VA_ARGS__))
# define MMK_LAST_43(Head, ...) MMK_EXPAND(MMK_LAST_42(__VA_ARGS__))
# define MMK_LAST_44(Head, ...) MMK_EXPAND(MMK_LAST_43(__VA_ARGS__))
# define MMK_LAST_45(Head, ...) MMK_EXPAND(MMK_LAST_44(__VA_ARGS__))
# define MMK_LAST_46(Head, ...) MMK_EXPAND(MMK_LAST_45(__VA_ARGS__))
# define MMK_LAST_47(Head, ...) MMK_EXPAND(MMK_LAST_46(__VA_ARGS__))
# define MMK_LAST_48(Head, ...) MMK_EXPAND(MMK_LAST_47(__VA_ARGS__))
# define MMK_LAST_49(Head, ...) MMK_EXPAND(MMK_LAST_48(__VA_ARGS__))
# define MMK_LAST_50(Head, ...) MMK_EXPAND(MMK_LAST_49(__VA_ARGS__))
# define MMK_LAST_51(Head, ...) MMK_EXPAND(MMK_LAST_50(__VA_ARGS__))
# define MMK_LAST_52(Head, ...) MMK_EXPAND(MMK_LAST_51(__VA_ARGS__))
# define MMK_LAST_53(Head, ...) MMK_EXPAND(MMK_LAST_52(__VA_ARGS__))
# define MMK_LAST_54(Head, ...) MMK_EXPAND(MMK_LAST_53(__VA_ARGS__))
# define MMK_LAST_55(Head, ...) MMK_EXPAND(MMK_LAST_54(__VA_ARGS__))
# define MMK_LAST_56(Head, ...) MMK_EXPAND(MMK_LAST_55(__VA_ARGS__))
# define MMK_LAST_57(Head, ...) MMK_EXPAND(MMK_LAST_56(__VA_ARGS__))
# define MMK_LAST_58(Head, ...) MMK_EXPAND(MMK_LAST_57(__VA_ARGS__))
# define MMK_LAST_59(Head, ...) MMK_EXPAND(MMK_LAST_58(__VA_ARGS__))
# define MMK_LAST_60(Head, ...) MMK_EXPAND(MMK_LAST_59(__VA_ARGS__))
# define MMK_LAST_61(Head, ...) MMK_EXPAND(MMK_LAST_60(__VA_ARGS__))
# define MMK_LAST_62(Head, ...) MMK_EXPAND(MMK_LAST_61(__VA_ARGS__))
# define MMK_LAST_63(Head, ...) MMK_EXPAND(MMK_LAST_62(__VA_ARGS__))
# define MMK_LAST_64(Head, ...) MMK_EXPAND(MMK_LAST_63(__VA_ARGS__))
# define MMK_LAST_65(Head, ...) MMK_EXPAND(MMK_LAST_64(__VA_ARGS__))

# define MMK_LAST__(n, ...) MMK_EXPAND(MMK_LAST_##n(__VA_ARGS__,))
# define MMK_LAST_(n, ...) MMK_EXPAND(MMK_LAST__(n, __VA_ARGS__))
# define MMK_LAST(...) MMK_EXPAND(MMK_LAST_(MMK_VA_NARGS(,__VA_ARGS__), __VA_ARGS__))

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

# define MMK_INC(N) MMK_INC_(N)
# define MMK_INC_(N) MMK_INC_ ## N
# define MMK_INC_0 1
# define MMK_INC_1 2
# define MMK_INC_2 3
# define MMK_INC_3 4
# define MMK_INC_4 5
# define MMK_INC_5 6
# define MMK_INC_6 7
# define MMK_INC_7 8
# define MMK_INC_8 9
# define MMK_INC_9 10
# define MMK_INC_10 11
# define MMK_INC_11 12
# define MMK_INC_12 13
# define MMK_INC_13 14
# define MMK_INC_14 15
# define MMK_INC_15 16
# define MMK_INC_16 17
# define MMK_INC_17 18
# define MMK_INC_18 19
# define MMK_INC_19 20
# define MMK_INC_20 21
# define MMK_INC_21 22
# define MMK_INC_22 23
# define MMK_INC_23 24
# define MMK_INC_24 25
# define MMK_INC_25 26
# define MMK_INC_26 27
# define MMK_INC_27 28
# define MMK_INC_28 29
# define MMK_INC_29 30
# define MMK_INC_30 31
# define MMK_INC_31 32
# define MMK_INC_32 33
# define MMK_INC_33 34
# define MMK_INC_34 35
# define MMK_INC_35 36
# define MMK_INC_36 37
# define MMK_INC_37 38
# define MMK_INC_38 39
# define MMK_INC_39 40
# define MMK_INC_40 41
# define MMK_INC_41 42
# define MMK_INC_42 43
# define MMK_INC_43 44
# define MMK_INC_44 45
# define MMK_INC_45 46
# define MMK_INC_46 47
# define MMK_INC_47 48
# define MMK_INC_48 49
# define MMK_INC_49 50
# define MMK_INC_50 51
# define MMK_INC_51 52
# define MMK_INC_52 53
# define MMK_INC_53 54
# define MMK_INC_54 55
# define MMK_INC_55 56
# define MMK_INC_56 57
# define MMK_INC_57 58
# define MMK_INC_58 59
# define MMK_INC_59 60
# define MMK_INC_60 61
# define MMK_INC_61 62
# define MMK_INC_62 63
# define MMK_INC_63 64

# define MMK_DEC(N) MMK_DEC_(N)
# define MMK_DEC_(N) MMK_DEC_ ## N
# define MMK_DEC_0 0
# define MMK_DEC_1 0
# define MMK_DEC_2 1
# define MMK_DEC_3 2
# define MMK_DEC_4 3
# define MMK_DEC_5 4
# define MMK_DEC_6 5
# define MMK_DEC_7 6
# define MMK_DEC_8 7
# define MMK_DEC_9 8
# define MMK_DEC_10 9
# define MMK_DEC_11 10
# define MMK_DEC_12 11
# define MMK_DEC_13 12
# define MMK_DEC_14 13
# define MMK_DEC_15 14
# define MMK_DEC_16 15
# define MMK_DEC_17 16
# define MMK_DEC_18 17
# define MMK_DEC_19 18
# define MMK_DEC_20 19
# define MMK_DEC_21 20
# define MMK_DEC_22 21
# define MMK_DEC_23 22
# define MMK_DEC_24 23
# define MMK_DEC_25 24
# define MMK_DEC_26 25
# define MMK_DEC_27 26
# define MMK_DEC_28 27
# define MMK_DEC_29 28
# define MMK_DEC_30 29
# define MMK_DEC_31 30
# define MMK_DEC_32 31
# define MMK_DEC_33 32
# define MMK_DEC_34 33
# define MMK_DEC_35 34
# define MMK_DEC_36 35
# define MMK_DEC_37 36
# define MMK_DEC_38 37
# define MMK_DEC_39 38
# define MMK_DEC_40 39
# define MMK_DEC_41 40
# define MMK_DEC_42 41
# define MMK_DEC_43 42
# define MMK_DEC_44 43
# define MMK_DEC_45 44
# define MMK_DEC_46 45
# define MMK_DEC_47 46
# define MMK_DEC_48 47
# define MMK_DEC_49 48
# define MMK_DEC_50 49
# define MMK_DEC_51 50
# define MMK_DEC_52 51
# define MMK_DEC_53 52
# define MMK_DEC_54 53
# define MMK_DEC_55 54
# define MMK_DEC_56 55
# define MMK_DEC_57 56
# define MMK_DEC_58 57
# define MMK_DEC_59 58
# define MMK_DEC_60 59
# define MMK_DEC_61 60
# define MMK_DEC_62 61
# define MMK_DEC_63 62
# define MMK_DEC_64 63

# define MMK_TOKEN_(Id, T) MMK_TOKEN_ ## Id ## _ ## T (_)
# define MMK_TOKEN(Id, T) MMK_TOKEN_(Id, T)

# define MMK_TOKEN_SELECT(Id, Macro1, Params1, Macro2, Params2, Tok)    \
    MMK_TOKEN_SELECT_HELPER(MMK_VA_TAIL_SELECT(MMK_TOKEN(Id, Tok) _),   \
                Macro1, Params1, Macro2, Params2)

# define MMK_TOKEN_SELECT_HELPER(N, ...)  MMK_EXPAND(MMK_TOKEN_SELECT_HELPER_(N, __VA_ARGS__))
# define MMK_TOKEN_SELECT_HELPER_(N, ...) MMK_EXPAND(MMK_TOKEN_SELECT_HELPER_##N(__VA_ARGS__))
# define MMK_TOKEN_SELECT_HELPER_1(Macro1, Params1, Macro2, Params2) MMK_EXPAND(Macro1 Params1)
# define MMK_TOKEN_SELECT_HELPER_2(Macro1, Params1, Macro2, Params2) MMK_EXPAND(Macro2 Params2)

# define MMK_PARAM_N_(P, N) P ## N
# define MMK_PARAM_N(P, N) MMK_PARAM_N_(P, N)

# define MMK_TOKEN_plist_mmk_va_args(...) ,

# define MMK_LAST_PARAM_VA(VA_T) VA_T(_)
# define MMK_LAST_PARAM_NORMAL(T, N) T MMK_PARAM_N(param, N)

# define MMK_PARAM_LIST_1(...)
# define MMK_PARAM_LIST_2(VA_T, N, T, ...) MMK_TOKEN_SELECT(plist, MMK_LAST_PARAM_NORMAL, (T, N), MMK_LAST_PARAM_VA, (VA_T), T)
# define MMK_PARAM_LIST_3(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_2(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_4(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_3(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_5(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_4(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_6(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_5(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_7(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_6(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_8(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_7(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_9(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_8(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_10(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_9(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_11(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_10(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_12(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_11(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_13(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_12(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_14(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_13(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_15(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_14(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_16(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_15(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_17(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_16(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_18(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_17(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_19(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_18(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_20(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_19(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_21(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_20(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_22(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_21(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_23(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_22(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_24(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_23(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_25(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_24(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_26(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_25(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_27(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_26(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_28(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_27(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_29(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_28(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_30(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_29(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_31(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_30(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_32(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_31(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_33(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_32(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_34(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_33(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_35(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_34(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_36(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_35(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_37(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_36(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_38(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_37(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_39(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_38(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_40(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_39(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_41(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_40(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_42(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_41(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_43(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_42(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_44(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_43(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_45(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_44(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_46(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_45(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_47(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_46(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_48(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_47(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_49(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_48(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_50(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_49(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_51(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_50(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_52(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_51(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_53(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_52(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_54(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_53(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_55(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_54(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_56(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_55(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_57(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_56(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_58(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_57(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_59(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_58(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_60(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_59(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_61(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_60(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_62(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_61(VA_T, MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_LIST_63(VA_T, N, T, ...) T MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_LIST_62(VA_T, MMK_INC(N), __VA_ARGS__))

# define MMK_PARAM_LIST__(VA_T, n, ...) MMK_EXPAND(MMK_PARAM_LIST_##n(VA_T, 0, __VA_ARGS__,))
# define MMK_PARAM_LIST_(VA_T, n, _, ...) MMK_EXPAND(MMK_PARAM_LIST__(VA_T, n, __VA_ARGS__))
# define MMK_PARAM_LIST_NE(VA_T, ...) MMK_EXPAND(MMK_PARAM_LIST_(VA_T, MMK_VA_NARGS(__VA_ARGS__), __VA_ARGS__))

# define MMK_PARAM_LIST_SELECT(VA_T, ...) MMK_EXPAND(MMK_PARAM_LIST_SELECT_HELPER(VA_T, MMK_VA_TAIL_SELECT(__VA_ARGS__), __VA_ARGS__))

# define MMK_PARAM_LIST_SELECT_HELPER(VA_T, N, ...)  MMK_EXPAND(MMK_PARAM_LIST_SELECT_HELPER_(VA_T, N, __VA_ARGS__))
# define MMK_PARAM_LIST_SELECT_HELPER_(VA_T, N, ...) MMK_EXPAND(MMK_PARAM_LIST_SELECT_HELPER_##N(VA_T, __VA_ARGS__))
# define MMK_PARAM_LIST_SELECT_HELPER_1(...) void
# define MMK_PARAM_LIST_SELECT_HELPER_2(VA_T, ...) MMK_EXPAND(MMK_PARAM_LIST_NE(VA_T, __VA_ARGS__))

# define MMK_PARAM_LIST(VA_T, ...) MMK_PARAM_LIST_SELECT(VA_T, __VA_ARGS__)

# define MMK_TOKEN_ppack_mmk_va_args(...) ,

# define MMK_LAST_PPACK_VA(...) vl_cpy__
# define MMK_LAST_PPACK_NORMAL(N) MMK_PARAM_N(param, N)

# define MMK_PARAM_PACK_1(...)
# define MMK_PARAM_PACK_2(N, T, ...) MMK_TOKEN_SELECT(ppack, MMK_LAST_PPACK_NORMAL, (N), MMK_LAST_PPACK_VA, (_), T)
# define MMK_PARAM_PACK_3(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_2(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_4(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_3(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_5(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_4(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_6(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_5(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_7(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_6(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_8(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_7(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_9(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_8(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_10(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_9(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_11(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_10(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_12(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_11(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_13(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_12(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_14(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_13(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_15(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_14(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_16(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_15(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_17(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_16(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_18(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_17(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_19(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_18(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_20(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_19(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_21(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_20(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_22(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_21(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_23(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_22(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_24(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_23(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_25(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_24(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_26(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_25(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_27(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_26(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_28(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_27(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_29(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_28(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_30(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_29(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_31(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_30(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_32(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_31(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_33(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_32(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_34(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_33(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_35(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_34(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_36(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_35(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_37(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_36(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_38(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_37(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_39(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_38(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_40(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_39(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_41(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_40(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_42(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_41(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_43(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_42(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_44(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_43(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_45(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_44(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_46(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_45(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_47(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_46(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_48(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_47(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_49(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_48(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_50(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_49(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_51(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_50(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_52(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_51(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_53(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_52(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_54(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_53(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_55(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_54(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_56(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_55(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_57(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_56(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_58(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_57(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_59(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_58(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_60(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_59(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_61(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_60(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_62(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_61(MMK_INC(N), __VA_ARGS__))
# define MMK_PARAM_PACK_63(N, T, ...) MMK_PARAM_N(param, N), MMK_EXPAND(MMK_PARAM_PACK_62(MMK_INC(N), __VA_ARGS__))

# define MMK_PARAM_PACK__(n, ...) MMK_EXPAND(MMK_PARAM_PACK_##n(0, __VA_ARGS__,))
# define MMK_PARAM_PACK_(n, _, ...) MMK_EXPAND(MMK_PARAM_PACK__(n, __VA_ARGS__))
# define MMK_PARAM_PACK_NE(...) MMK_EXPAND(MMK_PARAM_PACK_(MMK_VA_NARGS(__VA_ARGS__), __VA_ARGS__))

# define MMK_PARAM_PACK_SELECT(...) MMK_EXPAND(MMK_PARAM_PACK_SELECT_HELPER(MMK_VA_TAIL_SELECT(__VA_ARGS__), __VA_ARGS__))

# define MMK_PARAM_PACK_SELECT_HELPER(N, ...)  MMK_EXPAND(MMK_PARAM_PACK_SELECT_HELPER_(N, __VA_ARGS__))
# define MMK_PARAM_PACK_SELECT_HELPER_(N, ...) MMK_EXPAND(MMK_PARAM_PACK_SELECT_HELPER_##N(__VA_ARGS__))
# define MMK_PARAM_PACK_SELECT_HELPER_1(...)
# define MMK_PARAM_PACK_SELECT_HELPER_2(...) MMK_EXPAND(MMK_PARAM_PACK_NE(__VA_ARGS__))

# define MMK_PARAM_PACK(...) MMK_EXPAND(MMK_PARAM_PACK_SELECT(__VA_ARGS__))

/* We need a second version of MMK_TOKEN_SELECT because it would get painted
 * blue otherwise, and we can't exactly defer it (or at least, attempts to
 * do so failed so far).
 */
# define MMK_VOID_SELECT_TYPE_void() ,
# define MMK_VOID_SELECT_TYPE_(T) MMK_VOID_SELECT_TYPE_ ## T ()
# define MMK_VOID_SELECT_TYPE(T) MMK_VOID_SELECT_TYPE_(T)

# define MMK_VOID_SELECT(Macro, Params1, Params2, Type) MMK_EXPAND(MMK_VOID_SELECT_HELPER(MMK_VA_TAIL_SELECT(MMK_VOID_SELECT_TYPE(Type)), Macro, Params1, Params2))

# define MMK_VOID_SELECT_HELPER(N, ...)  MMK_EXPAND(MMK_VOID_SELECT_HELPER_(N, __VA_ARGS__))
# define MMK_VOID_SELECT_HELPER_(N, ...) MMK_EXPAND(MMK_VOID_SELECT_HELPER_##N(__VA_ARGS__))
# define MMK_VOID_SELECT_HELPER_1(Macro, Params1, Params2) MMK_EXPAND(Macro Params1)
# define MMK_VOID_SELECT_HELPER_2(Macro, Params1, Params2) MMK_EXPAND(Macro Params2)

# define MMK_COND_VOID(Macro, Params1, Params2, ...) MMK_EXPAND(MMK_VOID_SELECT(Macro, Params1, Params2, __VA_ARGS__))

# define MMK_VA_SELECT_TYPE_VAL_mmk_va_args() ,
# define MMK_VA_SELECT_TYPE_(T) MMK_VA_SELECT_TYPE_VAL_ ## T ()
# define MMK_VA_SELECT_TYPE(T) MMK_VA_SELECT_TYPE_(T)

# define MMK_VA_SELECT(Macro, Params1, Params2, Type) MMK_EXPAND(MMK_VA_SELECT_HELPER(MMK_VA_TAIL_SELECT(MMK_VA_SELECT_TYPE(Type)), Macro, Params1, Params2))

# define MMK_VA_SELECT_HELPER(N, ...)  MMK_EXPAND(MMK_VA_SELECT_HELPER_(N, __VA_ARGS__))
# define MMK_VA_SELECT_HELPER_(N, ...) MMK_EXPAND(MMK_VA_SELECT_HELPER_##N(__VA_ARGS__))
# define MMK_VA_SELECT_HELPER_1(Macro, Params1, Params2) MMK_EXPAND(Macro Params1)
# define MMK_VA_SELECT_HELPER_2(Macro, Params1, Params2) MMK_EXPAND(Macro Params2)

# define MMK_COND_VA(Macro, Params1, Params2, Tok) MMK_EXPAND(MMK_VA_SELECT(Macro, Params1, Params2, Tok))

#endif /* !MIMICK_PREPROCESS_H_ */
