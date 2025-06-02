#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "bmp.h"


int compute_polynomial(int shadow, int pol_size, int coefficients[],int* flag);
int create_shadows(uint64_t ** shadows, int n, int shadow_size);
void encrypt(int r, int n, BMPImage image,BMPImage shadows[]);
void decrypt(int r,BMPImage shadows[],char* imagePath);
#endif