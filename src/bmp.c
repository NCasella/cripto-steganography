#include "include/bmp.h"
#include "include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//BMPHeader and BMPImageStruct sourced from https://engineering.purdue.edu/ece264/16au/hw/HW13
typedef struct {             // Total: 54 bytes
    uint16_t  type;             // Magic identifier: 0x4d42
    uint32_t  size;             // File size in bytes
    uint16_t  reserved1;        // Not used
    uint16_t  reserved2;        // Not used
    uint32_t  offset;           // Offset to image data in bytes from beginning of file (54 bytes)
    uint32_t  dib_header_size;  // DIB Header size in bytes (40 bytes)
    int32_t   width_px;         // Width of the image
    int32_t   height_px;        // Height of image
    uint16_t  num_planes;       // Number of color planes
    uint16_t  bits_per_pixel;   // Bits per pixel
    uint32_t  compression;      // Compression type
    uint32_t  image_size_bytes; // Image size in bytes
    int32_t   x_resolution_ppm; // Pixels per meter
    int32_t   y_resolution_ppm; // Pixels per meter
    uint32_t  num_colors;       // Number of colors
    uint32_t  important_colors; // Important colors
} BMPHeader;

typedef struct {
    BMPHeader header;
    byte* data;
} BMPImageStruct;


BMPImage readImage(const char *filename) {
    FILE *imageFile = fopen(filename, "rb"); //TODO checkear rb
    BMPImage bmp = malloc(sizeof(BMPImageStruct));
    memcpy(&bmp->header, imageFile, HEADER_SIZE);
    bmp->data = malloc(sizeof(byte) * bmp->header.size);
    memcpy(&bmp->data, imageFile + HEADER_SIZE,  bmp->header.size);
    fclose(imageFile);
    return bmp;
}
