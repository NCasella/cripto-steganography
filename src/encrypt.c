#include "include/encrypt.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#define MAX 50
#define SET 10
/*variable global*/

int64_t seed; /*seed debe ser de 48 bits; se elige este tipo de 64 bits*/

void setSeed(int64_t seed);
uint8_t nextChar(void); /*devuelve un unsigned char*/
int compute_polynomial(int shadow);

int main(){
    int i;
    uint8_t num;
    setSeed(SET);
    for (i = 0; i < MAX; i++)
    {
        num = nextChar();
        printf("%d\t", num);
    }
    return EXIT_SUCCESS;
}
void setSeed(int64_t s){
    seed = (s ^ 0x5DEECE66DL) & ((1LL << 48) - 1);
}
uint8_t nextChar(void){
    seed = (seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
    return (uint8_t)(seed >> 40);
}

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

void encrypt(int r, int n){
    
    //genero n sombras -> n imagenes bmp
    //{puntero_bmp1,  puntero_bmp2, ...}
    
    int IMAGE_SIZE = 512;
    int shadow_size = IMAGE_SIZE / r;   //IMAGE_SIZE deberia ser global o recibirse por parametro
    int section_amount = shadow_size;
    uint8_t * shadows = {0}; //punteros a las sombras
    
    int coefficients[r] = {0}; //mod 251
    int offset = 0;

    int encrypted_pixels[n] = {0};
    int current_pixel = 0;

    //para todas las secciones de la foto original -> #secciones = sizeof(shadows)
    for(int j=0; j<r; j++){
        
        current_pixel = j; //cada nueva seccion de la foto D sera el siguiente pixel en las sombras 
                          //por eso hay IMAGE_SIZE/r = tamaño de sombras = #secciones
                          
        offset=j*r; //tamaño de la seccion * seccion actual (j)

        //creo el j-esimo polinomio
        for(int i=0; i<r; i++){
            polynomial[i]=bmp_image[offset+i]; //i-esimo pixel de la seccion j -> coeficiente del polinomio
        }
        for(int k=1; k<=n; k++){
            shadows[k-1][current_pixel] = compute_polynomial(k, coefficients);
        }
    }

}


int compute_polynomial(int shadow, int coefficients[r]){
    return shadow;
}