#include "include/encrypt.h"
#include "include/bmp.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#define MAX 50
#define SET 10
/*variable global*/

int compute_polynomial(int shadow, int pol_size, int coefficients[]);
int create_shadows(uint64_t ** shadows, int n, int shadow_size);
uint8_t getBitAt(uint8_t num,uint8_t bitPosition);
/*Forma de distribuir una foto en N sombras*/

 /*
 para un diagrama (r, n) de n sombras y r secretos:
 -genero n sombras --> imagenes bmp donde vamos a guardar los secretos
 -pensamos la imagen original D como un conjunto de secciones de r bits -> cada seccion se identifica con j
 
 
 -para cada seccion armamos un polinomio de grado r-1 -> puede ser un array de chars
    -cada coeficiente del polinomio sera el k-esimo pixel [0, r-1] de la seccion j
    
    -USAMOS R!!!!!


-se computan los pixeles que se guardaran en las sombras de esta forma:
    -q_j(1), q_j(2), q_j(3), ..., q_j(n)

    -USAMOS N!!!!!!
 
 */

void encrypt(int r, int n, char * imageName,BMPImage shadows[]){
    
    //genero n sombras -> n imagenes bmp
    //{puntero_bmp1,  puntero_bmp2, ...}

    BMPImage image = readImage(imageName);
    BMPHeader header;
    getHeaderCopy(image, &header);
    uint16_t obscuredImage[header.height_px * header.width_px];
    obscureImage(header.width_px, header.height_px, image, obscuredImage);

    //BMPPImage shadows[n];

    if(r == 8){
        encrypt_k8(n, header.width_px, header.height_px, getData(image) ,shadows);
    }



}

int compute_polynomial(int shadow, int pol_size, int * coefficients){
    int result=0;
    long long unsigned int partial = 0;
    for(int i=0; i<pol_size; i++){
        //caso especial p(i)=256
        partial = pow(shadow,i);
        result+= (coefficients[i] * partial%257);
    }
    int toReturn = result%257;
    return toReturn<0? toReturn+257 : toReturn;    
}

void encrypt_k8(int n,int width, int height, uint16_t obscuredImage[width * height],  BMPImage shadows[n]){
    int shadow_size = width * height; //tamaño de las sombras
    int coefficients[n]; //mod 257
    int pol_size = 0;
    int offset = 0;

    //para todas las secciones de la foto original -> #secciones = sizeof(shadows)
    for(int j=0; j<(shadow_size / 8); j++){    //TODO corregir: se esta asumiendo divisible por 8

        //cada nueva seccion de la foto Q sera el LSB del siguiente pixel en las sombras
        //por eso hay IMAGE_SIZE/r = tamaño de sombras = #secciones //TODO k!=8

        offset=j*8; //tamaño de la seccion * seccion actual (j)

        //creo el j-esimo polinomio
        pol_size=0;
        for(int i=0; i<8; i++){
            coefficients[i]=obscuredImage[offset+i]; //i-esimo pixel de la seccion j -> coeficiente del polinomio

            pol_size++;
        }
        printf("Current poly:");
        for(int m=0; m<pol_size;m++){
            printf("+%dx^%d", coefficients[m], m);
        }
        printf("\n");
        for(int k=1; k<=n; k++){
            printf("p(%d)=%d\n", k, compute_polynomial(k, pol_size,coefficients));
            //piso el j-esimo bit en la k-esima shadow
            //shadows[k-1][j] = compute_polynomial(k, coefficients);
        }
        printf("fin seccion %d, [%d, %d]\n", j, offset+1, offset+8);
        /*
        por cada j -> agarro qj(num)
        para cada S_num hago
        -> S_num[j][LSB] = qj[]
        */
        for(int shadow=0; shadow<n; shadow++){
            uint8_t p_x = compute_polynomial(shadow+1, 8,coefficients);
            uint8_t* shadowData=getData(shadows[shadow]);
            uint8_t result;
            
            for(uint8_t bit=0; bit<8; bit++){
                
                uint8_t b=getBitAt(p_x,bit);
                result=( shadowData[offset+bit] & 0xFe)|b; //cambio LS
                shadowData[offset+bit]=result;
            }
        }
    }
    for(int i=0;i<n;i++){
        char shadowName[]="shadow .bmp";
        shadowName[6]=i+'0';
        writeImage(shadows[i],shadowName);
    }
}

uint8_t getBitAt(uint8_t num,uint8_t bitPosition){
    uint8_t mask=1<<bitPosition;
    return (num&mask)>>bitPosition;
}


