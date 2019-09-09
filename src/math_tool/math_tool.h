#ifndef _MATH_TOOL_H
#define _MATH_TOOL_H

#include <stdint.h>

#define BIG_FLOAT 1.0e30;

// Union para hckear el exponente de un punto flotante tipo double
typedef union {
	double fp;
	struct {
		uint32_t lo;
		uint32_t hi;
	};
} hack_structure;


// float 32bit IEEE 754
// +---+------------------------+-------------------------------------------------------------------------------+
// | s |           e            |                                        m                                      |
// +---+------------------------+-------------------------------------------------------------------------------+
//  31  30                    24 23                                                                            0
// s = signo        1 bit
// e = exponente    8 bits
// m = mantisa      23 bits
typedef union {
	float fp;
    uint32_t dp;
    
	struct {
		unsigned int signo : 1;
        unsigned int expo : 8;
        unsigned int mantisa : 23;
	};
} hack_structure_f;

// Exponencial
float exp_f(float x);
double exp_lf(double x);

// Logaritmo natural
float ln_f(float x);
double ln_lf(double x);

// Potencia
float power_f(float x, uint32_t n);
float pow_f(float x, float y);

double power_lf(double x, uint32_t n);
double pow_lf(double x, double y);

#endif
