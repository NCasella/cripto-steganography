#include "include/polynomial.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
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

polynomial sumPolynomials(polynomial p1, polynomial p2) {

    polynomial result = NULL;
    polynomial lastPtr = result;

    while (p1 || p2) {
        polynomial newNode = malloc(sizeof(term));
        if (!newNode) {
            freePolynomial(result);
            return NULL;
        }

        if (p1 && (!p2 || p1->order > p2->order)) {
            newNode->coefficient = p1->coefficient;
            newNode->order = p1->order;
            p1 = p1->nextTerm;
        } else if (p2 && (!p1 || p2->order > p1->order)) {
            newNode->coefficient = p2->coefficient;
            newNode->order = p2->order;
            p2 = p2->nextTerm;
        } else {
            newNode->coefficient = p1->coefficient + p2->coefficient;
            newNode->order = p1->order;
            p1 = p1->nextTerm;
            p2 = p2->nextTerm;
        }

        newNode->nextTerm = NULL;
        lastPtr = newNode;
        lastPtr = newNode->nextTerm;
    }
    freePolynomial(p1);
    freePolynomial(p2);

    return result;
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

static int applyMod(int a,int mod){
    int result=a;
    if(result<0){
        result+=mod;
    }
    return result;
}

polynomial getLagrangePolynomial(int coefficients[][2],int coefficientSize,int mod){
    int s_i[coefficientSize];
    int p_i[coefficientSize];
    for(int i=0;i<coefficientSize;i++){
        p_i[i]=coefficients[i][1];
    }

    for(int i=0;i<coefficientSize;i++){
        s_i[coefficientSize-i-1]=getInterpolationValue(coefficients,coefficientSize-i,0,mod);
        for(int j=0;j<coefficientSize-i-1;j++){
            p_i[j]=(applyMod(s_i[coefficientSize-i-1]-p_i[j],mod)*modInverse(coefficients[j][0],mod))%mod;
        }
    }
    return newPolynomial(s_i,coefficientSize-1);

}



 int getInterpolationValue(int coefficients[][2],int order,int x,int mod){
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

