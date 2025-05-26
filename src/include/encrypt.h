#ifndef ENCRYPT_H
#define ENCRYPT_H
#include "bmp.h"

#define DUMMY_START 256
int compute_polynomial(int shadow, int pol_size, int coefficients[DUMMY_START]);
int create_shadows(uint64_t ** shadows, int n, int shadow_size);


#endif