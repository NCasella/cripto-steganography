#include "include/bmp.h"
#include "include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <asm-generic/errno-base.h>

typedef struct BMPImageStruct{
    BMPHeader* header;
    byte* data;
} BMPImageStruct;


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

    bmp->header = malloc(HEADER_SIZE);
    if (!bmp->header) {
        fclose(imageFile);
        free(bmp);
        return NULL;
    }

    if (fread(bmp->header, 1, HEADER_SIZE, imageFile) != HEADER_SIZE) {
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

    fclose(imageFile);
    return bmp;
}

BMPImage createBlankImage(uint32_t imageSize) {
    BMPImage bmp = malloc(sizeof(BMPImageStruct));
    bmp->header = malloc(HEADER_SIZE);
    bmp->header->image_size_bytes = imageSize;
    bmp->data = malloc(sizeof(byte) * imageSize);
    return bmp;
}

BMPImage createImageFromData(BMPHeader header, byte* data, uint32_t size) {
    BMPImage bmp = createBlankImage(size);
    memcpy(bmp->data, data, size);
    memcpy(bmp->header, &header, HEADER_SIZE);
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

BMPHeader* getHeader(BMPImage bmp) {
    return bmp->header;
}

byte* getData(BMPImage bmp) {
    return bmp->data;
}

