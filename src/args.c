#include <stdlib.h>
#include <stdio.h>
#include "include/args.h"
#include "include/bmp.h"
#include <getopt.h>
#include <dirent.h>
#include "include/bmp.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>

int getImagesInDirectory( char* directory,BMPImage* shadows,operationType op,int n,int k);

void parse_args(int argc,char** args, struct args* argStruct,BMPImage* shadows){
    bool distributeMode=0;
    bool recoverMode=0;
    int k=-1;
    int n=-1;
    char* imagePath=NULL;
    char* dirPath=".";

    static struct option long_opts[]={
        {"distribute",no_argument, NULL,'d'},
        {"recover", no_argument, NULL,'r'},
        {"k",required_argument, NULL,'k'},
        {"n",required_argument, NULL,'n'},
        {"secret",required_argument, NULL, 0xaffa},
        {"dir",required_argument, NULL, 0xcafe},
        {"help",no_argument,NULL,'h'}};

    int opt;
    int optIndex;


    while ((opt=getopt_long(argc,args,"drk:n:h",long_opts,&optIndex))!=-1){
        switch (opt){
        case 'd':
            distributeMode=true;
            break;
        case 'r':
            recoverMode=true;
            break;
        case 'k':
            k=atoi(optarg);
            break;
        case 'n':
            n=atoi(optarg);
            break;
        case 0xaffa:
            imagePath=optarg;
            break;
        case 0xcafe:
            dirPath=optarg;
            break;
        case 'h':
            printf("-d --decode        use distribute mode\n"
                    "-r --recover       use recover mode\n"
                    "-k <number>   minimum treshold\n"
                    "-n <number>   amount of files for\n"
                    "--secret <path>    path to image to distribute (if -d) or output path of recovered image (if -r)\n"
                    "--dir <path>   path to look for images\n"
                    "\n");
            exit(EXIT_SUCCESS);

        default:
            fprintf(stderr,"FATAL: unknown argument %c, use flag -h to see a list of available arguments\n",optopt);
            exit(EXIT_FAILURE);
        }
    }

    if(!(recoverMode ^ distributeMode)){
        fprintf(stderr,"FATAL: either -d OR -r must be specified\n");
        exit(EXIT_FAILURE);
    }
    if(k<2||k>10){
        fprintf(stderr,"FATAL: invalid k value\n");
        exit(EXIT_FAILURE);
    }
    if(distributeMode && n<2){
        fprintf(stderr,"FATAL: not enough images found on directory %s \n",dirPath);
    }
    if(distributeMode && k>n){
        fprintf(stderr,"FATAL: k greater than n value\n");
        exit(EXIT_FAILURE);
    }

    argStruct->operation=recoverMode?RECOVER:DISTRIBUTE;
    n=getImagesInDirectory(dirPath,shadows,argStruct->operation,n,k);
    if(n==-1){
        fprintf(stderr,"FATAL: not enough files for operation\n");
        exit(EXIT_FAILURE);
    }
    if(argStruct->operation==DISTRIBUTE){
        int ratios[] = {0,0,2,2,2,2,2,2,1,1,1};

        int ratio = ratios[k];
        BMPImage image=readImage(imagePath);
        if(image==NULL){
            fprintf(stderr,"FILE NON EXISTENT\n");
            goto freeImgs;
        }
        int32_t shadowHeight=image->header->height_px*ratio;
        int32_t shadowWidth=image->header->width_px*ratio;
        closeImage(image);
        for (int i=0; i<n; i++) {
            BMPImage aux = createImageFromData(shadows[i]->header, shadows[i]->data, shadowWidth, shadowHeight);
            closeImage(shadows[i]);
            shadows[i]=aux;
        }
    }
    argStruct->imagePath=imagePath;
    argStruct->directoryPath=dirPath;
    argStruct->k=k;
    argStruct->n=n;
    return;
    freeImgs:
    for(int i=0;i<n;i++){
        closeImage(shadows[i]);
    }
    exit(EXIT_FAILURE);
}


int getImagesInDirectory( char* directory,BMPImage* shadows,operationType op,int n,int k){
    struct dirent* dirent;
    DIR* dir=opendir(directory);
    int filesNeeded=k;
    int filesInDir=0;
    if(dir==NULL){
        fprintf(stderr,"FATAL: directory does not exist\n");
        exit(EXIT_FAILURE);
    }
    char buffer[255];
    while((dirent=readdir(dir))!=NULL){
        char* extension=strrchr(dirent->d_name,'.');
        if(extension!=NULL && strcmp(extension,".bmp")==0){
            strcpy(buffer,directory);
            strcat(buffer,"/");
            strcat(buffer,dirent->d_name);
            shadows[filesInDir]=readImage(buffer);
            if(shadows[filesInDir]==NULL){
                closedir(dir);
                fprintf(stderr,"FATAL: something went wrong reading a .bmp file\n");
                goto fileError;
            }
            filesInDir++;
            filesNeeded--;
            if((op==RECOVER && filesNeeded==0)|| (op==DISTRIBUTE && n!=-1&&filesInDir==n))
               break;
        }
    }
    closedir(dir);
    for (int i=0; i<filesInDir-1; i++) {
        if (shadows[i]->header->height_px != shadows[i + 1]->header->height_px ||
        shadows[i]->header->width_px != shadows[i + 1]->header->width_px) {
            fprintf(stderr, "Files in directory have different sizes");
            goto fileError;
        }
    }
    if((op==RECOVER && k!=filesInDir)||(op==DISTRIBUTE && filesInDir!=n)){
        fprintf(stderr,"Not enough files for image reveal");
        fileError:
        for(int i=0;i<filesInDir;i++){
            closeImage(shadows[i]);
        }

        exit(EXIT_FAILURE);
    }
    return filesInDir;
}