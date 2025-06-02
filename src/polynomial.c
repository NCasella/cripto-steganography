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

static uint8_t applyMod(int a,int mod){
    int result=a;
    if(result<0){
        result+=mod;
    }
    uint8_t res= result%mod;
    return res==mod? res:res;
}


void getLagrangePolynomialCoefficients(uint8_t points[][2],int pointSize,int mod,uint8_t coefficients[]){
    uint8_t s[pointSize][2];
    memcpy(s,points,pointSize*2);
    for(int i=0;i<pointSize;i++){
        coefficients[i]=getInterpolationValue(s,pointSize-i,0,mod);
        for(int p=0;p<pointSize-i-1;p++){
            s[p][1]=applyMod(s[p][1]-coefficients[i],mod)*modInverse(applyMod(s[p][0],mod),mod)%mod;
        }
    }

}



uint8_t getInterpolationValue(uint8_t coefficients[][2],int order,int x,int mod){
    int result=0;
    for(int i=0;i<order;i++){
        int p=1;
        for (int j=0;j<order;j++){
            if(j!=i){
                p=p*applyMod(x-coefficients[j][0],mod)*modInverse(applyMod(coefficients[i][0]-coefficients[j][0],mod),mod)%mod;
            }
        }
        result+=p*coefficients[i][1]%mod;
    }
    return applyMod(result,mod);
}

