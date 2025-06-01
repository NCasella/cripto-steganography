#ifndef SEEDS_H
#define SEEDS_H

#include <stdlib.h>
#include <stdint.h>
#include "bmp.h"


void setSeed(int64_t s);
uint8_t nextChar(void);

/**
 * Creates an encrypted matrix of pixels given a bmp image.
 * This function assumes a correct seed inicialization.
 * The matrix created must be freed after use with freeMatrix().
 * 
 * @param width     the width of the image.
 * @param height    the height of the image.
 * @param image     the bmp image.
 * @return a matrix of pixels.
 */
void obscureImage(uint16_t width, uint16_t height, BMPImage image, uint16_t toReturn[width*height]);
void freeMatrix(uint16_t ** matrix);

#endif /* SEEDS_H */