#include "math_tool.h"

double exp_lf(double x) {
    return 0.0;
}

// Calcula el logaritmo natural de un punto flotante en el rango 0..1
// Method: Limit the range to z = 0.069
// Use minimaxed 5th-order rational
// polynomial to calculate the log.
static double __ln_lf(double x) {
	const double a = 1;
	const double b = -0.2672435;
	const double c = -0.600576;

	double z = (x - 1) / (x + 1);
	double z2 = z * z;
	return 2 * z * (a + b * z2) / (1 + c * z2);
}

// Parte de segmentacion en el cálculo del logaritmo
// x esta en el rango 0.5 .. 1.0
static double _ln_lf(double x) {
	const double limit1 = 0.879559563;	// 0.5^(1/5)
	const double limit2 = 0.659753955;	// 0.5^(3/5)
	const double k1 = 0.757858283;		// 0.5^(2/5)
	const double k2 = 0.574349177;		// 0.5^(4/5)
	const double ln_k = -0.138629436;	// ln(0.5^(1/5))

	if( x >= limit1) return __ln_lf(x);
	if( x >= limit2) return __ln_lf(x/k1) + 2 * ln_k;
	return __ln_lf(x / k2) + 4 * ln_k;
}

double ln_lf(double x) {
	const double ln2 = 0.69314718; // log(2);
	int16_t expo;
	double retval;
	hack_structure _x;

	if( x <= 0) return -BIG_FLOAT;
	_x.fp = x;

	// extraer el exponente
	expo = (int16_t)(_x.hi >> 20);
	expo -= 0x3fe;

	// force the exponent to zero
	_x.hi &= 0x000fffff;
	_x.hi += 0x3fe00000;

	// compute the log of the mantissa only
	retval = _ln_lf(_x.fp);

	// rebuild the result
	//retval = ln2 * (expo + retval / ln2);
	retval = ln2 * expo + retval; // ¿porque no mejor esto en lugar de la línea de arriba?
	return retval;
}

double pow_lf(double x, double y) {
    return exp_lf(y * ln_lf(x));
}



