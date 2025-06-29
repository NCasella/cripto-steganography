#ifndef ARGS_H
#define ARGS_H
#include "bmp.h"
#include <stdbool.h>

typedef enum operationType {DISTRIBUTE ,RECOVER} operationType;

struct args{
    operationType operation;
    char* imagePath;
    char* directoryPath;
    int n;
    int k;
};

void parse_args(int argc,char** args,struct args* argStruct,BMPImage* shadows);

#endif  /* ARGS_H */