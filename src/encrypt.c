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
#define true 1
#define false 0
/*variable global*/

int ratios[] = {0,0,2,2,2,2,2,2,1,1,1};
int checkShadowSize(int r, int n, BMPImage shadows[], int imageSize);
void encryptProcess(int n,int k, int width, int height, const uint8_t obscuredImage[], BMPImage shadows[n]);

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
    setSeed(SET);
    //genero n sombras -> n imagenes bmp
    //{puntero_bmp1,  puntero_bmp2, ...}
    for(int i=0;i<n;i++){
        shadows[i]->header->seed=(uint16_t)SET;
    }
    uint8_t obscuredImage[image->header->height_px * image->header->width_px];
    obscureImage(image->header->width_px, image->header->height_px, image, obscuredImage);

    //BMPImage shadows_2[n];

    int imageSize = image->header->height_px * image->header->width_px;
    if (!checkShadowSize(r, n, shadows, imageSize)){
        //TODO hacer todos los frees necesarios
        return;
    }

    encryptProcess(n, r ,image->header->width_px, image->header->height_px, obscuredImage ,shadows);

}

int compute_polynomial(int shadow, int polSize, uint8_t * coefficients,int* flag){
    int result=0;
    long long unsigned int partial = 0;
    for(int i=0; i<polSize; i++){
        //caso especial p(i)=256
        partial = intPowMod(shadow,i);
        result+= (coefficients[i] * partial%MOD);
    }
    int toReturn = result%MOD;
    if(toReturn==MOD-1)
        *flag=1;
    return toReturn<0? toReturn+MOD : toReturn;
}




void decrypt_k8(int k,int width,int height, BMPImage shadows[],char* imagePath){
    BMPHeader * header = malloc(shadows[0]->header->offset);
    getHeaderCopy(shadows[0],header);
    BMPImage revealedImg=createImageFromData(header, shadows[0]->data, width, height);
    free(header);
    int shadowSize=width*height;
    uint8_t* imgData=getData(revealedImg);
    int bitOffset=0;
    for(int j=0;j<shadowSize/k;j++){
        int offset=j*k;
        uint8_t coeffs[k];
        uint8_t pointsY[k];
        uint16_t pointsX[k];
        for(int shadow=0;shadow<k;shadow++){
            uint8_t point=0;
            for(int i=0;i<8;i++){
                uint8_t shadowByte=getByte(shadows[shadow],bitOffset+i);
                point=point<<1|getBitAt(shadowByte,0);//LSB

            }
            printf("\n");
            printf("%d\n",point);
            pointsX[shadow]=shadows[shadow]->header->shadowNumber;
            pointsY[shadow]=point;
        }
        bitOffset += 8;
        getInterpolationCoefficients(pointsX, pointsY, k, MOD, coeffs);
        for(int i=0;i<k;i++){
            imgData[offset+i]=coeffs[i]^nextChar();
        }
    }
    writeImage(revealedImg,imagePath);
    closeImage(revealedImg);

}

void decrypt(int r, BMPImage shadows[],char* imagePath) {
    setSeed(shadows[0]->header->seed);
    decrypt_k8(r,shadows[0]->header->width_px / ratios[r], shadows[0]->header->height_px / ratios[r], shadows,imagePath);
}

void encryptProcess(int n,int k, int width, int height, const uint8_t obscuredImage[], BMPImage shadows[n]){
    uint8_t coefficients[k]; //mod 257
    int polSize = 0;
    int offset = 0;
    int bitOffset = 0;

    for(int j=0; j<( width * height)/k; j++){
        offset=j*k; //tamaño de la seccion * seccion actual (j)

        //creo el j-esimo polinomio
        polSize=0;
        for(int i=0; i<k; i++){
            coefficients[i]=obscuredImage[offset+i]; //i-esimo pixel de la seccion j -> coeficiente del polinomio

            polSize++;
        }
        for(int N=1; N<=n; N++){
            int flag=0;
            //piso el j-esimo bit en la k-esima shadow
            //shadows[k-1][j] = compute_polynomial(k, coefficients);
        }

       uint8_t p_xi[n];
        for(int shadow=0; shadow<n; shadow++){
            int flag256=0;
            uint8_t p_x = compute_polynomial(shadow+1, k,coefficients,&flag256);
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
                result=(shadowData[bitOffset+bit] & 0xFe)|b; //cambio LS
                shadowData[bitOffset+bit]=result;
            }
        }
        bitOffset+=8;
    }
    for(uint16_t i=0;i<n;i++){
        char shadowName[]="shadow .bmp";
        shadowName[6]=i+'0';
        shadows[i]->header->shadowNumber=i+1;
        writeImage(shadows[i],shadowName);
    }
}

uint8_t getBitAt(uint8_t num,uint8_t bitPosition){
    uint8_t mask=1<<bitPosition;
    return (num&mask)>>bitPosition;
}

int checkShadowSize(int r, int n, BMPImage shadows[], int imageSize) {
    int correctShadowSize = (ratios[r] * ratios[r]) * imageSize;
    int shadowSize = 0;
    for(int i=0; i<n; i++) {
        shadowSize = shadows[i]->header->height_px * shadows[i]->header->width_px;
        if (shadowSize != correctShadowSize) {
            printf("Image %d does not have the correct size. Find another that does.\n", i);
            return false;
        }
    }
    return true;
}


