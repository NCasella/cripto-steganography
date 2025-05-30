#include "seeds.h"
#include "include/bmp.h"

uint64_t seed;

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

void obscureMatrix(uint16_t width, uint16_t height, BMPImage image, uint16_t toReturn[height][width]) {
    uint8_t currentChar = nextChar();
    uint8_t pixel;

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){

            pixel = getByte(image, i, j);
            if(pixel>=0) toReturn[i][j] = getByte(image, i, j) ^ currentChar;       //^ bitwise XOR

            currentChar = nextChar();
        }
    }
}

void freeMatrix(uint16_t ** matrix){
    if(matrix != NULL){
        free(matrix);
    }
}

