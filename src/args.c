#include <stdlib.h>
#include <stdio.h>
#include "include/args.h"
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>

static int getImagesInDirectory( char* directory);

void parse_args(int argc,char** args, struct args* argStruct){
    bool distributeMode=0;
    bool recoverMode=0;
    int k=-1;
    int n=-1;
    char* imagePath=NULL;
    char* dirPath=".";

    static struct option long_opts[]={
        {"decode",no_argument, NULL,'d'},
        {"recover", no_argument, NULL,'r'},
        {"k",required_argument, NULL,'k'},
        {"n",required_argument, NULL,'n'},
        {"secret",required_argument, NULL, 0xaffa},
        {"dir",required_argument, NULL, 0xcafe}};
    
    int opt;
    int optIndex;


    while ((opt=getopt_long(argc,args,"drk:n:",long_opts,&optIndex))!=-1){
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
        default:
            fprintf(stderr,"FATAL: unknown argument %c\n",optopt);
            exit(EXIT_FAILURE);
        }
    }

    if(n==-1){
        n=getImagesInDirectory(dirPath);
    }
    /*   FIXME: descomentar luego de testear 
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
    */
    argStruct->operation=recoverMode?recoverMode:distributeMode;
    argStruct->imagePath=imagePath;
    argStruct->directoryPath=dirPath;
    argStruct->k=k;
    argStruct->n=n;

    return;
}

static int getImagesInDirectory( char* directory){
    struct dirent* dirent;
    DIR* dir=opendir(directory);

    int n=0;
    if(dir==NULL){
        fprintf(stderr,"FATAL: directory does not exist\n");
        exit(EXIT_FAILURE);
    }
    while((dirent=readdir(dir))!=NULL){
        char* extension=strrchr(dirent->d_name,'.');
        if(extension!=NULL && strcmp(extension,".bmp"))
            n++;
    }
    return n;
}