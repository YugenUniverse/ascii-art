#ifndef PPM_H
#define PPM_H
#include "image/image.h"
#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)

typedef struct {
  uint16_t ppmType;
  uint32_t ppmWidth;
  uint32_t ppmHeight;
  uint16_t ppmMaxColorValue;
} PPMHEADER;

#pragma pack(pop)

image_t *PPM_conversion(const char *filename);
PPMHEADER *readHeader(const char *filename);
void skip_comments_whitespaces(FILE *fp);
uint8_t normalizeColorChannel(uint16_t c, uint16_t ppmMaxColorValue);
void skip_header_data_for_read(FILE *fp);

#endif // !BMP_H
