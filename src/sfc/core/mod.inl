#pragma once

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc99-extensions"
#pragma clang diagnostic ignored "-Wc11-extensions"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wc++98-c++11-c++14-compat"

#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wgnu-statement-expression"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#pragma clang diagnostic ignored "-Wpragma-once-outside-header"
#pragma clang diagnostic ignored "-Wunused-macros"
#endif

/* clang-format off */
#define SFC_COMMA ,
#define SFC_SEMI  ;
#define SFC_ARG(...)    __VA_ARGS__

#define SFC_LOOP_0(F, X)
#define SFC_LOOP_1(F, X)  F(0)
#define SFC_LOOP_2(F, X)  SFC_LOOP_1(F, SFC_ARG(X))X F(1)
#define SFC_LOOP_3(F, X)  SFC_LOOP_2(F, SFC_ARG(X))X F(2)
#define SFC_LOOP_4(F, X)  SFC_LOOP_3(F, SFC_ARG(X))X F(3)
#define SFC_LOOP_5(F, X)  SFC_LOOP_4(F, SFC_ARG(X))X F(4)
#define SFC_LOOP_6(F, X)  SFC_LOOP_5(F, SFC_ARG(X))X F(5)
#define SFC_LOOP_7(F, X)  SFC_LOOP_6(F, SFC_ARG(X))X F(6)
#define SFC_LOOP_8(F, X)  SFC_LOOP_7(F, SFC_ARG(X))X F(7)
#define SFC_LOOP_9(F, X)  SFC_LOOP_8(F, SFC_ARG(X))X F(8)
#define SFC_LOOP_10(F, X) SFC_LOOP_9(F, SFC_ARG(X))X F(9)
#define SFC_LOOP_11(F, X) SFC_LOOP_10(F, SFC_ARG(X))X F(10)
#define SFC_LOOP_12(F, X) SFC_LOOP_11(F, SFC_ARG(X))X F(11)
#define SFC_LOOP_13(F, X) SFC_LOOP_12(F, SFC_ARG(X))X F(12)
#define SFC_LOOP_14(F, X) SFC_LOOP_13(F, SFC_ARG(X))X F(13)
#define SFC_LOOP_15(F, X) SFC_LOOP_14(F, SFC_ARG(X))X F(14)
#define SFC_LOOP_16(F, X) SFC_LOOP_15(F, SFC_ARG(X))X F(15)
#define SFC_LOOP_17(F, X) SFC_LOOP_16(F, SFC_ARG(X))X F(16)
#define SFC_LOOP_18(F, X) SFC_LOOP_17(F, SFC_ARG(X))X F(17)
#define SFC_LOOP_19(F, X) SFC_LOOP_18(F, SFC_ARG(X))X F(18)
#define SFC_LOOP_20(F, X) SFC_LOOP_19(F, SFC_ARG(X))X F(19)
#define SFC_LOOP_21(F, X) SFC_LOOP_20(F, SFC_ARG(X))X F(20)
#define SFC_LOOP_22(F, X) SFC_LOOP_21(F, SFC_ARG(X))X F(21)
#define SFC_LOOP_23(F, X) SFC_LOOP_22(F, SFC_ARG(X))X F(22)
#define SFC_LOOP_24(F, X) SFC_LOOP_23(F, SFC_ARG(X))X F(23)
#define SFC_LOOP_25(F, X) SFC_LOOP_24(F, SFC_ARG(X))X F(24)
#define SFC_LOOP_26(F, X) SFC_LOOP_25(F, SFC_ARG(X))X F(25)
#define SFC_LOOP_27(F, X) SFC_LOOP_26(F, SFC_ARG(X))X F(26)
#define SFC_LOOP_28(F, X) SFC_LOOP_27(F, SFC_ARG(X))X F(27)
#define SFC_LOOP_29(F, X) SFC_LOOP_28(F, SFC_ARG(X))X F(28)
#define SFC_LOOP_30(F, X) SFC_LOOP_29(F, SFC_ARG(X))X F(29)
#define SFC_LOOP_31(F, X) SFC_LOOP_30(F, SFC_ARG(X))X F(30)
#define SFC_LOOP_32(F, X) SFC_LOOP_31(F, SFC_ARG(X))X F(31)

#define SFC_LOOP_EXPR(N, F) SFC_LOOP_##N(F, SFC_COMMA)
#define SFC_LOOP_STMT(N, F) SFC_LOOP_##N(F, SFC_SEMI)
#define SFC_LOOP(N, F, ...)   SFC_LOOP_##N(F, __VA_ARGS__)

/* clang-format on */
