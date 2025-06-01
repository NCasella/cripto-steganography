#include "./src/include/bmp.h"
#include "./src/include/encrypt.h"
#include "src/include/utils.h"
#include "src/include/args.h"
#include "src/include/encrypt.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    setSeed(123456789); // Set a fixed seed for reproducibility
    BMPHeader header;
    BMPImage image = readImage("input/Alfred.bmp");
    //struct args arg;
    //parse_args(argc,argv,&arg);
    getHeaderCopy(image, &header);

    printf("Photo bytes\n");
    for(int j=0; j<10; j++){
        printf("%x ", getByte(image, j));
    }
    puts("obscured bytes\n");

    uint16_t byteMatrix[header.height_px][header.width_px];
    obscureImage(header.width_px, header.height_px, image, byteMatrix);
    //int n=getImagesInDirectory("input/");
    BMPImage shadows[10];
    for(int i=0;i<10;i++){
        char imgName[]="blanks/blank-image .bmp";
        imgName[11]=i+'0';

        char name[] = "shadow .bmp";
        name[6] = i +'0';
        BMPImage img = readImage(name);
        //writeImage(img,imgName);
        shadows[i]=img;
    }

    printf("%d", image->header->offset);
    
    //encrypt(8,10,image,shadows);

    decrypt(8, shadows);

    for(int i=0; i<10; i++) {
        closeImage(shadows[i]);
    }

    closeImage(image);
    return 0;

}