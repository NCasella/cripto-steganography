#include <stdint.h>
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H






void getInterpolationCoefficients(const uint16_t *x, const uint8_t *y, int pointSize, int mod, uint8_t coefficients[]);

#endif 