#include <stdlib.h>
#include <stdio.h>
#include "include/args.h"
#include <getopt.h>


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
        {"dir",optional_argument, NULL, 0xcafe}};
    
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
            fprintf(stderr,"ERROR: unknown argument %c\n",opt);
            exit(EXIT_FAILURE);
        }
    }


    if(!(recoverMode ^ distributeMode)){
        fprintf(stderr,"ERROR: either -d OR -r must be specified\n");
        exit(EXIT_FAILURE);
    }
    if(k<2||k>10){
        
        fprintf(stderr,"ERROR: invalid k value\n");
        exit(EXIT_FAILURE);
    }
    if(k>n){
        fprintf(stderr,"ERROR: k greater than n value\n");
        exit(EXIT_FAILURE);
    }
    
    argStruct->operation=recoverMode?recoverMode:distributeMode;
    argStruct->imagePath=imagePath;
    argStruct->directoryPath=dirPath;
    argStruct->k=k;
    argStruct->n=n;

    return;
}