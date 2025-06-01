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

int evaluatePolynomial(polynomial p,int x){
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

static int modInverse(int a,int mod) {
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
    return result;
}

void getLagrangePolynomialCoefficients(int points[][2],int pointSize,int mod,uint8_t coefficients[]){
    int s_i[pointSize];
    int p_i[pointSize];
    for(int i=0;i<pointSize;i++){
        p_i[i]=points[i][1];
    }
    for(int i=0;i<pointSize;i++){
        s_i[i]=getInterpolationValue(points,pointSize-i,0,mod);
        for(int j=0;j<pointSize-i-1;j++){
            p_i[j]=(applyMod(s_i[i]-p_i[j],mod)*modInverse(points[j][0],mod))%mod;
        }
    }
    for(int i=0;i<pointSize;i++){
        coefficients[i]=s_i[i];
    }

}



uint8_t getInterpolationValue(int coefficients[][2],int order,int x,int mod){
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

