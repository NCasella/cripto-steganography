#include "seeds.h"
#include "include/bmp.h"

int64_t seed;

void
setSeed(int64_t s)
{
seed = (s ^ 0x5DEECE66DL) & ((1LL << 48) - 1);
}
uint8_t
nextChar(void){
seed = (seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
return (uint8_t)(seed>>40);
}
uint16_t getSeed(){
    return seed;
}

void obscureImage(uint16_t width, uint16_t height, BMPImage image, uint8_t toReturn[width*height]) {
    uint8_t currentChar = nextChar();
    uint8_t pixel;

   for(int i=0; i< (height * width); i++){
        pixel = getByte(image, i);
        if(pixel>=0)toReturn[i] = pixel ^ currentChar;       //^ bitwise XOR
        else 
            *NULL;

        currentChar = nextChar();
    }
    
}

void freeMatrix(uint16_t ** matrix){
    if(matrix != NULL){
        free(matrix);
    }
}

