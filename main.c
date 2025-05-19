#include "src/include/bmp.h"
#include "src/include/utils.h"
#include <stdio.h>


int main(int argc, char **argv) {
    printf("Reading file\n");
    BMPImage bmp = readImage("src/input/image1.bmp");
    printf("Writing file\n");
    writeImage(bmp, "src/output/image1_out.bmp");
    printf("Closing file\n");
    closeImage(bmp);
    return 0;
}