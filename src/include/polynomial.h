#include <stdint.h>
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

typedef struct term* polynomial; 
/*
len(coefficients)==order+1
coefficientes ordenados de mayor orden a menor
coefficients=[4,-5,6], order=2
p(x)=4x^2-5x+6
*/
polynomial newPolynomial(int* coefficients,int order);

//evalua p(x)
uint8_t evaluatePolynomial(polynomial p,uint8_t x);

//multiplica los coeficientes del polinomio p por x, inplace. Devuelve p 
polynomial multiplyPolynomial(polynomial p ,int x);

void freePolynomial(polynomial p);

void getLagrangePolynomialCoefficients(const uint16_t *x, const uint8_t *y, int pointSize, int mod, uint8_t coefficients[]);

#endif 