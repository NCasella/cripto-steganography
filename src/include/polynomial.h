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
int evaluatePolynomial(polynomial p,int x);

//multiplica los coeficientes del polinomio p por x, inplace. Devuelve p 
polynomial multiplyPolynomial(polynomial p ,int x);

void freePolynomial(polynomial p);

polynomial sumPolynomials(polynomial p1, polynomial p2);

int getInterpolationValue(int coefficients[][2],int order,int x,int mod);

#endif 