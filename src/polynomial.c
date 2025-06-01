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

static uint8_t modInverse(int a,int mod) {
    for (int i = 1; i < mod; i++)
        if ((a * i) % mod == 1)
            return i;
    //printf("my final message, goodbye\n");
    //*NULL;
    return -1;
}

static uint8_t applyMod(int a,int mod){
    int result=a;
    if(result<0){
        result+=mod;
    }
    uint8_t res= result%mod;
    return res==mod-1? res-1:res;
}
void polyMultiply(uint8_t result[], uint8_t p[], int xi, int mod, int degree) {
    uint8_t temp[10] = {0};
    for (int i = 0; i <= degree; i++) {
        temp[i + 1] = p[i];
        temp[i] = (temp[i] - (int64_t)p[i] * xi % mod + mod) % mod;
    }
    memcpy(result, temp, sizeof(uint8_t) * (degree + 2));
}

void getLagrangePolynomialCoefficients(uint8_t points[][2],int pointSize,int mod,uint8_t coefficients[]){
    uint8_t basis[10];
    uint8_t poly[10];
    uint8_t temp[10];
    memset(coefficients,0,pointSize);

    for (int i = 0; i < pointSize; i++) {
        int xi = points[i][0];
        int yi = points[i][1];
        int denom = 1;

        // Initialize basis poly to [1]
        memset(basis, 0, sizeof(basis));
        basis[0] = 1;
        int degree = 0;

        for (int j = 0; j < pointSize; j++) {
            if (j == i) continue;
            int xj = points[j][0];
            denom = (int64_t)denom * (xi - xj + mod) % mod;
            polyMultiply(temp, basis, xj, mod, degree);
            degree++;
            memcpy(basis, temp, sizeof(uint8_t) * (degree + 1));
        }

        int denomInv = modInverse(denom, mod);
        for (int k = 0; k <= degree; k++) {
            int term = (int64_t)basis[k] * yi % mod;
            term = (int64_t)term * denomInv % mod;
            coefficients[k] = applyMod(coefficients[k] + term, mod);
        }
    }

}



uint8_t getInterpolationValue(uint8_t coefficients[][2],int order,int x,int mod){
    int result=0;
    for(int i=0;i<order;i++){
        int p=1;
        for (int j=0;j<order;j++){
            if(j!=i){
                p=p*applyMod(x-coefficients[j][0],mod)*modInverse(coefficients[i][0]-coefficients[j][0],mod);
            }
        }
        result+=p*coefficients[i][1];
    }
    return applyMod(result,mod);
}

