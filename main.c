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
    struct args arg;
    BMPImage shadows[10];
    parse_args(argc,argv,&arg,shadows);
    
    BMPImage image=readImage(arg.imagePath);
    if(arg.operation==DISTRIBUTE)
        encrypt(arg.k,arg.n,image,shadows);
    else if(arg.operation==RECOVER)
        decrypt(8, shadows);

    for(int i=0; i<10; i++) {
        closeImage(shadows[i]);
    }
    closeImage(image);
    return 0;

}