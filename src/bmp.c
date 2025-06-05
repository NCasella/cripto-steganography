#include "include/bmp.h"
#include "include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <asm-generic/errno-base.h>

BMPImage readImage(const char *filename) {
    FILE *imageFile = fopen(filename, "rb");
    if (!imageFile) {
        perror("Error opening file");
        return NULL;
    }

    BMPImage bmp = malloc(sizeof(BMPImageStruct));
    if (!bmp) {
        fclose(imageFile);
        return NULL;
    }

    BMPHeader* auxHeader = malloc(HEADER_SIZE);

    fread(auxHeader, 1, HEADER_SIZE, imageFile);

    bmp->header = malloc(auxHeader->offset);
    if (!bmp->header) {
        fclose(imageFile);
        free(bmp);
        return NULL;
    }

    rewind(imageFile);

    if (fread(bmp->header, 1, auxHeader->offset, imageFile) != auxHeader->offset) {
        perror("Error reading header");
        fclose(imageFile);
        free(bmp->header);
        free(bmp);
        return NULL;
    }

    if (fseek(imageFile, bmp->header->offset, SEEK_SET) != 0) {
        perror("Error seeking to pixel data");
        fclose(imageFile);
        free(bmp->header);
        free(bmp);
        return NULL;
    }

    if (bmp->header->image_size_bytes == 0) {
        bmp->header->image_size_bytes = bmp->header->width_px * bmp->header->height_px * (bmp->header->bits_per_pixel / 8);
    }

    bmp->data = malloc(bmp->header->image_size_bytes);
    if (!bmp->data) {
        fclose(imageFile);
        free(bmp->header);
        free(bmp);
        return NULL;
    }

    if (fread(bmp->data, 1, bmp->header->image_size_bytes, imageFile) != bmp->header->image_size_bytes) {
        perror("Error reading image data");
        fclose(imageFile);
        free(bmp->data);
        free(bmp->header);
        free(bmp);
        return NULL;
    }

    free(auxHeader);
    fclose(imageFile);
    return bmp;
}

BMPImage createBlankImage(uint32_t imageSize, uint32_t headerSize) {
    BMPImage bmp = malloc(sizeof(BMPImageStruct));
    bmp->header = malloc(headerSize);
    bmp->header->image_size_bytes = imageSize;
    bmp->header->offset = headerSize;
    bmp->data = calloc(1,sizeof(byte) * imageSize);
    bmp->header->size = imageSize + headerSize;
    return bmp;
}

BMPImage createImageCopy(BMPImage image) {
    return createImageFromData(image->header, image->data, image->header->image_size_bytes, image->header->width_px, image->header->height_px);
}

BMPImage createImageFromData(BMPHeader* header, byte* data, uint32_t size, uint32_t width, uint32_t height) {
    BMPImage bmp = calloc(1, sizeof(BMPImageStruct));
    bmp->header = calloc(1, header->offset);
    bmp->data = calloc(1, sizeof(byte) * size);
    size_t copySize = (header->image_size_bytes > size) ? size : header->image_size_bytes;
    memcpy(bmp->data, data, copySize);
    memcpy(bmp->header, header, header->offset);
    bmp->header->image_size_bytes = size;
    bmp->header->size = size + header->offset;
    bmp->header->width_px = width;
    bmp->header->height_px = height;

    return bmp;
}


int _mkdirs(const char *path) {
    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            struct stat st;
            if (stat(tmp, &st) != 0) {
                if (mkdir(tmp, 0755) != 0) {
                    if (errno != EEXIST) {
                        return -1;
                    }
                }
            } else if (!S_ISDIR(st.st_mode)) {
                return -1;
            }
            *p = '/';
        }
    }

    struct stat st;
    if (stat(tmp, &st) != 0) {
        if (mkdir(tmp, 0755) != 0) {
            if (errno != EEXIST) return -1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        return -1;
    }
    return 0;
}

void writeImage(BMPImage bmp, const char *filename) {
    char dirpath[1024];
    strncpy(dirpath, filename, sizeof(dirpath));
    dirpath[sizeof(dirpath) - 1] = '\0';

    char *last_slash = strrchr(dirpath, '/');
    if (last_slash) {
        *last_slash = '\0';
        if (_mkdirs(dirpath) != 0) {
            fprintf(stderr, "Failed to create directories for path: %s\n", dirpath);
            return;
        }
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return;
    }

    if (fwrite(bmp->header, 1, bmp->header->offset, file) != bmp->header->offset) {
        fprintf(stderr, "Failed to write header to file: %s\n", filename);
        fclose(file);
        return;
    }

    size_t pixel_data_size = bmp->header->image_size_bytes;
    if (pixel_data_size == 0) {
        pixel_data_size = bmp->header->size - bmp->header->offset;
    }

    if (fwrite(bmp->data, 1, pixel_data_size, file) != pixel_data_size) {
        fprintf(stderr, "Failed to write image data to file: %s\n", filename);
        fclose(file);
        return;
    }

    fclose(file);
}

void closeImage(BMPImage bmp) {
    free(bmp->data);
    free(bmp->header);
    free(bmp);
}

void getHeaderCopy(BMPImage bmp, BMPHeader* toCopy) {
    memcpy(toCopy, bmp->header, bmp->header->offset);
}

uint32_t getHeaderSize(BMPImage bmp) {
    return bmp->header->offset;
}

byte* getData(BMPImage bmp) {
    return bmp->data;
}

uint8_t getByte(BMPImage image, int index){
	byte * data = image->data;
    int width = image->header->width_px;
    int height = image->header->height_px;

    if (index < 0 || index >= width * height) {
        return -1; // Invalid index
    }
    return data[index];
}
