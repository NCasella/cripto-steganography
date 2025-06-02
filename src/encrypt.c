#include "include/encrypt.h"
#include "include/bmp.h"
#include "include/polynomial.h"
#include "include/seeds.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#define MAX 50
#define SET 10
#define MOD 257
/*variable global*/

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
static int intPowMod(int base, int exp) {
    long long acc = 1;
   for(int i=0;i<exp;i++){
    acc*=base%MOD;
   }
   return acc%MOD;
}
void encrypt(int r, int n, BMPImage image,BMPImage shadows[]){
    
    //genero n sombras -> n imagenes bmp
    //{puntero_bmp1,  puntero_bmp2, ...}

    uint8_t obscuredImage[image->header->height_px * image->header->width_px];
    obscureImage(image->header->width_px, image->header->height_px, image, obscuredImage);

    //BMPImage shadows_2[n];

    if(r == 8){
        encrypt_k8(n, image->header->width_px, image->header->height_px, obscuredImage ,shadows);
    }

}

int compute_polynomial(int shadow, int pol_size, uint8_t * coefficients,int* flag){
    int result=0;
    long long unsigned int partial = 0;
    for(int i=0; i<pol_size; i++){
        //caso especial p(i)=256
        partial = intPowMod(shadow,i);
        result+= (coefficients[i] * partial%MOD);
    }
    int toReturn = result%MOD;
    if(toReturn==MOD-1)
        *flag=1;
    return toReturn<0? toReturn+MOD : toReturn;
}

int cmp(const void *a, const void *b) {
    const BMPImage *ia = (const BMPImage *)a;
    const BMPImage *ib = (const BMPImage *)b;
    return ((*ia)->header->shadowNumber - (*ib)->header->shadowNumber);
}



void decrypt_k8(int width,int height, BMPImage shadows[],char* imagePath){
    BMPImage revealedImg=createImageCopy(shadows[0]);
    const int k=8;
    qsort(shadows,k,sizeof(BMPImage),cmp);
    int shadowSize=width*height;
    uint8_t* imgData=getData(revealedImg);
    for(int j=0;j<shadowSize/k;j++){
        int offset=j*k;
        uint8_t coeffs[k];
        uint8_t pointsY[k];
        uint16_t pointsX[k];
        printf("seccion%d\n",j);
        for(int shadow=0;shadow<k;shadow++){
            uint8_t point=0;
            printf("shadowak sjfbjaf: %d\n",shadow);
            for(int i=0;i<k;i++){
                uint8_t shadowByte=getByte(shadows[shadow],offset+i);
                point=point<<1|getBitAt(shadowByte,0);//LSB
                printf("%d",getBitAt(shadowByte,0));

            }
            printf("\n");
            printf("%d\n",point);
            pointsX[shadow]=shadow+1;
            pointsY[shadow]=point;
        }
        getLagrangePolynomialCoefficients(pointsX, pointsY, k, MOD, coeffs);
        for(int i=0;i<k;i++){
            imgData[offset+i]=coeffs[i];//^nextChar();
            printf("coefficient %d: %d en hex:%x\n",i,coeffs[i],coeffs[i]);
        }
    }
    writeImage(revealedImg,imagePath);
    closeImage(revealedImg);

}

void decrypt(int r, BMPImage shadows[],char* imagePath) {
    decrypt_k8(shadows[0]->header->width_px, shadows[0]->header->height_px, shadows,imagePath);
}

void encrypt_k8(int n,int width, int height, const uint8_t obscuredImage[],  BMPImage shadows[n]){
    int shadow_size = width * height; //tamaño de las sombras
    uint8_t coefficients[8]; //mod 257
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
            printf("OFFSET: %d\n", offset+i);
            coefficients[i]=obscuredImage[offset+i]; //i-esimo pixel de la seccion j -> coeficiente del polinomio

            pol_size++;
        }
        printf("Current poly:");
        for(int m=0; m<pol_size;m++){
            printf("+% x^%d", coefficients[m], m);
        }
        printf("\n");
        for(int k=1; k<=n; k++){
            int flag=0;
            printf("p(%d)=%d\n", k, compute_polynomial(k, pol_size,coefficients,&flag));
            //piso el j-esimo bit en la k-esima shadow
            //shadows[k-1][j] = compute_polynomial(k, coefficients);
        }
        printf("fin seccion %d, [%d, %d]\n", j, offset+1, offset+8);
        /*
        por cada j -> agarro qj(num)
        para cada S_num hago
        -> S_num[j][LSB] = qj[]
        */
       uint8_t p_xi[n];
        for(int shadow=0; shadow<n; shadow++){
            int flag256=0;
            uint8_t p_x = compute_polynomial(shadow+1, 8,coefficients,&flag256);
            if(flag256){
                for(int i=0;i<8;i++){
                    if(coefficients[i]!=0){
                        coefficients[i]--;
                        shadow=-1;
                        break;//resetear el for 
                    }
                }
                continue;
            }
            p_xi[shadow]=p_x;
        }
        for(int shadow=0;shadow<n;shadow++){
            uint8_t* shadowData=getData(shadows[shadow]);
            uint8_t result;
            for(uint8_t bit=0; bit<8; bit++){
                uint8_t b=getBitAt(p_xi[shadow],8-bit-1);
                result=(shadowData[offset+bit] & 0xFe)|b; //cambio LS
                shadowData[offset+bit]=result;
            }
        }
    }
    for(uint16_t i=0;i<n;i++){
        char shadowName[]="shadow .bmp";
        shadowName[6]=i+'0';
        shadows[i]->header->shadowNumber=i;
        writeImage(shadows[i],shadowName);
    }
}

uint8_t getBitAt(uint8_t num,uint8_t bitPosition){
    uint8_t mask=1<<bitPosition;
    return (num&mask)>>bitPosition;
}


