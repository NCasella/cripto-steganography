#include "./src/include/bmp.h"
#include "./src/include/encrypt.h"
#include "src/include/polynomial.h"
#include "src/include/utils.h"
#include "src/include/args.h"
#include "src/include/seeds.h"
#include "src/include/encrypt.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    setSeed(12348); // Set a fixed seed for reproducibility
    struct args arg;
    BMPImage shadows[10];
    BMPImage image;
    parse_args(argc,argv,&arg,shadows);

    if(arg.operation==DISTRIBUTE){
        image=readImage(arg.imagePath);
        encrypt(arg.k,arg.n,image,shadows);
    }
    else if(arg.operation==RECOVER)

        decrypt(arg.k, shadows,arg.imagePath);

    for(int i=0; i<arg.n; i++) {
        closeImage(shadows[i]);
    }
    if(arg.operation==DISTRIBUTE){
        closeImage(image);
    }
    return 0;


}