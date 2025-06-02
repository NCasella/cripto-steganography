#include "include/polynomial.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct term {
    int coefficient;
    int order;
    struct term* nextTerm;
} term;

typedef term* polynomial;

#define MAX_DEGREE 10


static polynomial newPolynomialRec(int* coefficients,int order,bool* mallocNulled){
    if(order<0){
        return NULL;
    }
    polynomial newTerm=malloc(sizeof(term));
    if(newTerm==NULL){
        *mallocNulled=true;
        return NULL;
    }
    newTerm->coefficient=*coefficients;
    newTerm->order=order;
    newTerm->nextTerm=newPolynomialRec(coefficients+1,order-1,mallocNulled);
    return newTerm;

}
polynomial newPolynomial(int* coefficients,int order){
    bool mallocNulled=false;
    polynomial firstTerm= newPolynomialRec(coefficients,order,&mallocNulled);
    if(mallocNulled){
        freePolynomial(firstTerm);
        return NULL;
    }
    return firstTerm;
}

polynomial multiplyPolynomial(polynomial p, int x){
    polynomial aux=p;
    while(aux!=NULL){
        aux->coefficient*=x;
        aux=aux->nextTerm;
    }
    return p;
}

uint8_t evaluatePolynomial(polynomial p,uint8_t x){
    polynomial currentTerm=p;
    int px=0;
    while(currentTerm!=NULL){
        px+=currentTerm->coefficient*pow(x,currentTerm->order);
        currentTerm=currentTerm->nextTerm;
    }
    return px;
}


void freePolynomial(polynomial p){
    polynomial aux=p;
    while(aux!=NULL){
        polynomial next=aux->nextTerm;
        free(aux);
        aux=next;
    }
}

int modInverse(int a, int mod) {
    int t = 0, new_t = 1;
    int r = mod, new_r = a;

    while (new_r != 0) {
        int quotient = r / new_r;
        int temp_t = t;
        t = new_t;
        new_t = temp_t - quotient * new_t;

        int temp_r = r;
        r = new_r;
        new_r = temp_r - quotient * new_r;
    }

    if (r > 1) {
        *NULL;
        return -1;} // Not invertible
    if (t < 0) t += mod;
    return t;
}

int applyMod(int64_t value, int mod) {
    int result = value % mod;
    return result < 0 ? result + mod : result;
}

void polyMultiply(uint8_t* dest, uint8_t* poly, int xj, int mod, int degree) {
    memset(dest, 0, MAX_DEGREE);
    for (int i = degree; i >= 0; i--) {
        // dest[i+1] += -xj * poly[i]
        dest[i + 1] = applyMod(dest[i + 1] + poly[i], mod);
        dest[i] = applyMod(dest[i] - xj * poly[i], mod);
    }
}

void getLagrangePolynomialCoefficients(const uint16_t *x, const uint8_t *y, int pointSize, int mod, uint8_t coefficients[]) {
    uint16_t matrix[MAX_DEGREE][MAX_DEGREE + 1];
    memset(coefficients, 0, pointSize);

    for (int i = 0; i < pointSize; i++) {
        uint16_t xi_pow = 1;
        for (int j = 0; j < pointSize; j++) {
            matrix[i][j] = xi_pow;
            xi_pow = (xi_pow * x[i]) % mod;
        }
        matrix[i][pointSize] = y[i];
    }

    for (int col = 0; col < pointSize; col++) {
        int pivot = -1;
        for (int row = col; row < pointSize; row++) {
            if (matrix[row][col] != 0) {
                pivot = row;
                break;
            }
        }

        if (pivot == -1) {
            printf("Error: Singular matrix - no pivot found\n");
            return;
        }

        if (pivot != col) {
            for (int j = 0; j <= pointSize; j++) {
                uint16_t temp = matrix[col][j];
                matrix[col][j] = matrix[pivot][j];
                matrix[pivot][j] = temp;
            }
        }

        uint16_t inv = modInverse(matrix[col][col], mod);
        for (int j = col; j <= pointSize; j++) {
            matrix[col][j] = ((uint32_t)matrix[col][j] * inv) % mod;
        }

        for (int row = col + 1; row < pointSize; row++) {
            uint16_t factor = matrix[row][col];
            for (int j = col; j <= pointSize; j++) {
                matrix[row][j] = applyMod(matrix[row][j] - factor * matrix[col][j], mod);
            }
        }
    }

    for (int i = pointSize - 1; i >= 0; i--) {
        int64_t sum = matrix[i][pointSize];
        for (int j = i + 1; j < pointSize; j++) {
            sum = applyMod(sum - matrix[i][j] * coefficients[j], mod);
        }
        coefficients[i] = applyMod(sum, mod);
    }
}