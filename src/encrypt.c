#include "include/encrypt.h"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#define MAX 50
#define SET 10
/*variable global*/

int compute_polynomial(int shadow, int pol_size, int coefficients[DUMMY_START]);
int create_shadows(uint64_t ** shadows, int n, int shadow_size);

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

void encrypt(int r, int n, char * imageName){
    
    //genero n sombras -> n imagenes bmp
    //{puntero_bmp1,  puntero_bmp2, ...}

    BMPImage image = readImage(imageName);
    BMPHeader header;
    getHeaderCopy(image, &header);
    uint16_t byteMatrix[header.height_px][header.width_px];
    obscureMatrix(header.width_px, header.height_px, image, byteMatrix);

    BMPPImage shadows[n];

    if(r == 8){
        encrypt_k8(n, byteMatrix, header.width_px, header.height_px, shadows);
    }

    int IMAGE_SIZE = 512;
    int shadow_size = IMAGE_SIZE / r;   //IMAGE_SIZE deberia ser global o recibirse por parametro
    uint64_t * shadows[DUMMY_START] = {0};
    //int res = create_shadows(shadows, n, r); //punteros a las sombras
    
    
    int coefficients[DUMMY_START] = {0}; //mod 251
    int pol_size = 0;
    int offset = 0;

    int encrypted_pixels[DUMMY_START] = {0};

    //para todas las secciones de la foto original -> #secciones = sizeof(shadows)
    for(int j=0; j<shadow_size; j++){
        
        //cada nueva seccion de la foto D sera el siguiente pixel en las sombras 
        //por eso hay IMAGE_SIZE/r = tamaño de sombras = #secciones
                          
        offset=j*r; //tamaño de la seccion * seccion actual (j)

        //creo el j-esimo polinomio
        pol_size=0;
        for(int i=0; i<r; i++){
            //coefficients[i]=bmp_image[offset+i]; //i-esimo pixel de la seccion j -> coeficiente del polinomio
            coefficients[i]=i;
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
        printf("fin seccion %d, [%d, %d]\n", j, offset+1, offset+r);
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

int create_shadows(uint64_t ** shadows, int n, int shadow_size){

    char filename[50] = "./src/output/0";
    char nbr = '1';
    //asumimos n y r validados
    for(int i=0; i<n; i++){
        //shadows[i] = createBlankImage(shadow_size);
        writeImage(shadows[i], filename);
        nbr=nbr+i;
        filename[13]=nbr;  
    }
    return 0;
}