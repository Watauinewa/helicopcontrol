#include "math_tool.h"

// Exponencial
static float _exp_f(float x);
static double _exp_lf(double x);

// Logaritmo natural
static float __ln_f(float x);
static float _ln_f(float x);

static double __ln_lf(double x);
static double _ln_lf(double x);

// Calcula la potencia de e^x usando la serie de taylor en orden 5 y
// reduciendo el rango con m = 1/4, optimizando los coeficientes con minimax
// (x debe ser siempre mayor que 0)
static float _exp_f(float x) {
    const float root_e = 1.284025416687741;
	const float a = 1.000000034750736;
	const float b = 0.499997989964957;
	const float c = 0.166704077809886;
	const float d = 0.041365419657829;
	const float e = 9.419273457583088E-3;
	float retval;
	float intpart;
	int32_t n;

    x *= 4;
	n = (int32_t)x;
	x -= n;
    x /= 4;

	retval = (((( x * e + d) * x + c) * x + b) * x + a) * x + 1;
	intpart = power_f(root_e, n);

	return intpart * retval;
}

// Calcula e^x
float exp_f( float x) {
	if( x < 0) return 1 / _exp_f(-x);
	return _exp_f(x);
}

// Calcula la potencia de e^x usando la serie de taylor en orden 5 y
// reduciendo el rango con m = 1/4, optimizando los coeficientes con minimax
// (x debe ser siempre mayor que 0)
static double _exp_lf(double x) {
	const double root_e = 1.284025416687741;
	const double a = 1.000000034750736;
	const double b = 0.499997989964957;
	const double c = 0.166704077809886;
	const double d = 0.041365419657829;
	const double e = 9.419273457583088E-3;
	double retval;
	double intpart;
	int32_t n;

	x *= 4;
	n = (int32_t)x;
	x -= n;
	x /= 4;

	retval = (((( x * e + d) * x + c) * x + b) * x + a) * x + 1;
	intpart = power_lf(root_e, n);

	return intpart * retval;
}

// Calcula e^x
double exp_lf( double x) {
	if( x < 0) return 1 / _exp_lf(-x);
	return _exp_lf(x);
}

// Calcula el logaritmo natural de un punto flotante en el rango 1.587 ... 2
// Use minimaxed 5th-order rational
// polynomial to calculate the log.
static float __ln_f(float x) {
	const float a = 1;
	const float b = -0.2672435;
	const float c = -0.600576;

	float z = (x - 1) / (x + 1);
	float z2 = z * z;
	return 2 * z * (a + b * z2) / (1 + c * z2);
}

// Parte de segmentacion en el cálculo del logaritmo
// x esta en el rango 1.0 .. 2.0
static float _ln_f(float x) {
	const float k1 = 1.2599210498948732;       // 2^(1/3)
	const float k2 = 1.5874010519681994;       // 2^(2/3)
	const float ln_k1 = 0.2310490601866484;    // ln(k1)
	const float ln_k2 = 0.4620981203732968;    // ln(k2)

	if( x >= k2) return __ln_f(x);
	if( x >= k1) return __ln_f(x * k1) - ln_k1;
	return __ln_f(x * k2) - ln_k2;
}

// Calcula el logaritmo natural de x
float ln_f(float x) {
	const float ln2 = 0.69314718; // log(2);
	int16_t expo;
	float retval;
	hack_structure_f _x;

	if( x <= 0) return -BIG_FLOAT;
	_x.fp = x;

	// extraer el exponente
	expo = ((int32_t)_x.dp & 0x7f800000) >> 23;
	expo -= 0x7f;
	// force the exponent to zero
	_x.dp &= 0x007fffff;
	_x.dp |= 0x3f800000;

	// compute the log of the mantissa only
	retval = _ln_f(_x.fp);

	// rebuild the result
	//retval = ln2 * (expo + retval / ln2);
	retval = ln2 * expo + retval; // ¿porque no mejor esto en lugar de la línea de arriba?
	return retval;
}

// Calcula el logaritmo natural de un punto flotante en el rango 1.587 ... 2
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
// x esta en el rango 1.0 .. 2.0
static double _ln_lf(double x) {
// 	const double limit1 = 0.879559563;	// 0.5^(1/5)
// 	const double limit2 = 0.659753955;	// 0.5^(3/5)
// 	const double k1 = 0.757858283;		// 0.5^(2/5)
// 	const double k2 = 0.574349177;		// 0.5^(4/5)
// 	const double ln_k = -0.138629436;	// ln(0.5^(1/5))
// 
// 	if( x >= limit1) return __ln_lf(x);
// 	if( x >= limit2) return __ln_lf(x/k1) + 2 * ln_k;
// 	return __ln_lf(x / k2) + 4 * ln_k;
    
    const double k1 = 1.2599210498948732;       // 2^(1/3)
	const double k2 = 1.5874010519681994;       // 2^(2/3)
	const double ln_k1 = 0.2310490601866484;    // ln(k1)
	const double ln_k2 = 0.4620981203732968;    // ln(k2)

	if( x >= k2) return __ln_lf(x);
	if( x >= k1) return __ln_lf(x * k1) - ln_k1;
	return __ln_lf(x * k2) - ln_k2;
}

// Calcula el logaritmo natural de x
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

// Calcula la potencia de un numero cualquiera x a un numero entero n
float power_f( float x, uint32_t n) {
	float retval = 1;
	float factor = x;

	while( n != 0) {
		if( (n & 1) != 0) {
			retval *= factor;
		}

		factor *= factor;
		n >>= 1;
	}
	return retval;
}

float pow_f(float x, float y) {
    if( x >= 0) return exp_f(y * ln_f(x));
    
    // Chequear si "y" es impar o par y conello cambiar signo o no del retorno
    // *** falta verificar
    if( (int32_t)y % 2 == 0) return exp_f(y * ln_f(-x));
    return -exp_f( y * ln_f(-x));
}

// Calcula la potencia de un numero cualquiera x a un numero entero n
double power_lf( double x, uint32_t n) {
	double retval = 1;
	double factor = x;

	while( n != 0) {
		if( (n & 1) != 0) {
			retval *= factor;
		}

		factor *= factor;
		n >>= 1;
	}
	return retval;
}

// Calcula la potencia x^y
double pow_lf(double x, double y) {
    if(x >= 0) return exp_lf(y * ln_lf(x));
    
    // Chequear si "y" es impar o par y con ello cambiar signo o no del retorno
    // *** falta verificar
    if( (int64_t)y % 2 == 0) return exp_lf(y * ln_lf(-x));
    return -exp_lf(y * ln_lf(-x));
}




