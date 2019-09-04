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

double exp_lf(double x);
static double __ln_lf(double x);
static double _ln_lf(double x)
double ln_lf(double x);
double pow_lf(double x, double y);

#endif
