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

    setSeed(123456789); // Set a fixed seed for reproducibility
    struct args arg;
    BMPImage shadows[10];
    BMPImage image;
    parse_args(argc,argv,&arg,shadows);

    //TODO SACAME DE ACA LOCO!!!!
    image=readImage(arg.imagePath);




    //generate 10 shadows of size 8/k * imgsize

    //los ratios que hay que usar para K = 2,3,4,5,6,7,8,9,10
    int ratios[] = {0,0,2,2,2,2,2,2,1,1,1};

    //int imageSize = image->header->height_px * image->header->width_px;
    int ratio = ratios[arg.k];
    /*
    uint32_t shadowHeight=image->header->height_px*ratio;
    uint32_t shadowWidth=image->header->width_px*ratio;
    uint32_t shadowSize = shadowHeight*shadowWidth;
    char name[] = "chorizo .bmp";

    for (int i=0; i<10; i++) {
        shadows[i] = createImageFromData(image->header, image->data, shadowSize, shadowWidth, shadowHeight);
        name[7] = '0'+i;
        writeImage(shadows[i], name);
    }
*/
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