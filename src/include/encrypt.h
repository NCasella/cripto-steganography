#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "bmp.h"


int encrypt(int k, int n, BMPImage toEncrypt);
int decrypt(int k, BMPImage toDecrypt);


#endif