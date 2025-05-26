#include "src/include/bmp.h"
#include "src/include/utils.h"
#include "src/include/args.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char **argv) {
        
    struct args argsStruct;

    parse_args(argc,argv,&argsStruct);
    printf("Reading file\n");
    BMPImage bmp = readImage(argsStruct.imagePath);
    printf("Writing file\n");
    writeImage(bmp, "src/output/image1_out.bmp");
    printf("Closing file\n");
    closeImage(bmp);
    
    return 0;

}