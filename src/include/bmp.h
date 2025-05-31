#ifndef CRIPTO_STEGANOGRAPHY_BMPADT_H
#define CRIPTO_STEGANOGRAPHY_BMPADT_H

#include <stdint.h>
#include "utils.h"

#define HEADER_SIZE 54

//BMPHeader and BMPImageStruct sourced from https://engineering.purdue.edu/ece264/16au/hw/HW13
#pragma pack(push, 1) //Removes padding
typedef struct {
    uint16_t  type;
    uint32_t  size;
    uint16_t  seed;
    uint16_t  shadowNumber;
    uint32_t  offset;
    uint32_t  dib_header_size;
    int32_t   width_px;
    int32_t   height_px;
    uint16_t  num_planes;
    uint16_t  bits_per_pixel;
    uint32_t  compression;
    uint32_t  image_size_bytes;
    int32_t   x_resolution_ppm;
    int32_t   y_resolution_ppm;
    uint32_t  num_colors;
    uint32_t  important_colors;
} BMPHeader;
#pragma pack(pop)

typedef struct BMPImageStruct* BMPImage;

BMPImage readImage(const char *filename);

BMPImage createBlankImage(uint32_t imageSize, uint32_t headerSize);

BMPImage createImageFromData(BMPHeader* header, byte* data, uint32_t size, uint32_t width, uint32_t height);

void writeImage(BMPImage bmp, const char *filename);

void closeImage(BMPImage bmp);

void getHeaderCopy(BMPImage bmp, BMPHeader* toCopy);

uint32_t getHeaderSize(BMPImage bmp);

byte* getData(BMPImage bmp);

/**
 * Returns the pixel value at a specific row and column in the BMP image.
 *
 * @param width     the width of the image.
 * @param height    the height of the image.
 * @param image     the bmp image.
 * @return the pixel value or -1 if an error ocurred.
 */
uint8_t getByte(BMPImage image, int index);

#endif //CRIPTO_STEGANOGRAPHY_BMPADT_H
