#include "./src/include/bmp.h"
#include "./src/include/encrypt.h"
#include "src/include/utils.h"
#include "src/include/args.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    setSeed(123456789); // Set a fixed seed for reproducibility
    BMPHeader header;
    BMPImage image = readImage("src/input/image1.bmp");
    //struct args arg;
    //parse_args(argc,argv,&arg);
    getHeaderCopy(image, &header);

    printf("Photo bytes\n");
    for(int j=0; j<10; j++){
        printf("%x ", getByte(image, 0, j));
    }
    puts("obscured bytes\n");

    uint16_t byteMatrix[header.height_px][header.width_px];
    obscureMatrix(header.width_px, header.height_px, image, byteMatrix);
    int n=getImagesInDirectory("src/input/");


    closeImage(image);
    return 0;

}