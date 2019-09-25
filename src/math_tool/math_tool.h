#ifndef _MATH_TOOL_H
#define _MATH_TOOL_H

#include <stdint.h>

#define BIG_FLOAT 1.0e30;

// Union para hackear el exponente de un punto flotante tipo double
// According to IEEE 754 standard, the floating-point number is represented in following ways:
//     Half Precision (16 bit): 1 sign bit, 5 bit exponent, and 10 bit mantissa
//     Single Precision (32 bit): 1 sign bit, 8 bit exponent, and 23 bit mantissa
//     Double Precision (64 bit): 1 sign bit, 11 bit exponent, and 52 bit mantissa
//     Quadruple Precision (128 bit): 1 sign bit, 15 bit exponent, and 112 bit mantissa
//
// Single Precision:
//  float 32bit IEEE 754
//  +---+------------------------+-------------------------------------------------------------------------------+
//  | s |           e            |                                        m                                      |
//  +---+------------------------+-------------------------------------------------------------------------------+
//   31  30                    24 23                                                                            0
//  s = signo        1 bit
//  e = exponente    8 bits
//  m = mantisa      23 bits
//
//     The Exp field gives the exponent of the number, as a power of two. It is biased by 0x7F (127), so that very 
//     small numbers have exponents near zero and very large numbers have exponents near 0xFF (255).
//
//     If Exp = 0x7D (125), the number is between 0.25 and 0.5 (not including 0.5).
//     If Exp = 0x7E (126), the number is between 0.5 and 1.0 (not including 1.0).
//     If Exp = 0x7F (127), the number is between 1.0 and 2.0 (not including 2.0).
//     If Exp = 0x80 (128), the number is between 2.0 and 4.0 (not including 4.0).
//     If Exp = 0x81 (129), the number is between 4.0 and 8.0 (not including 8.0).
//
// Ejemplos
//     Float value    S     Exp     Frac         Mathematical value
//     0x3F800000     0     0x7F    000...000    1.0
//     0xBF800000     1     0x7F    000...000    -1.0
//     0x3F800001     0     0x7F    000...001    1.000 000 119
//     0x3F400000     0     0x7E    100...000    0.75
//     0x00800000     0     0x01    000...000    1.18*10^-38
//     0x00000001     0     0x00    000...001    1.40*10^-45
//     0x7F7FFFFF     0     0xFE    111...111    3.40*10^38
//     0x7F800000     0     0xFF    000...000    Plus infinity
//     0xFF800000     1     0xFF    000...000    Minus infinity
//     0x00000000     0     0x00    000...000    0.0
//     0x7F800001     0     0xFF    000...001    Signaling NaN
//     0x7FC00000     0     0xFF    100...000    Quiet NaN
//
typedef union {
    float fp;
    uint32_t dp;
} hack_structure_32f;
// Double Prevision:
//     The Exp field is biased by 0x3FF (1023) instead of 0x7F, so numbers between 1.0 and 2.0 have an Exp field of 0x3FF.
//     The Exp value representing infinity and NaNs is 0x7FF (2047) instead of 0xFF.
typedef union {
    double fp;
    struct {
        uint32_t lo;
        uint32_t hi;
    };
} hack_structure_64f;

// Exponencial
float exp_32f(float x);
double exp_64f(double x);

// Logaritmo natural
float ln_32f(float x);
double ln_64f(double x);

// Potencia
float power_32f(float x, uint32_t n);
float pow_32f(float x, float y);

double power_64f(double x, uint32_t n);
double pow_64f(double x, double y);

#endif
