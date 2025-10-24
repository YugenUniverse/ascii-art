#include "image/ppm.h"
#include "image/image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

image_t *PPM_conversion(const char *filename) {
  PPMHEADER *header = readHeader(filename);
  if (!header)
    return NULL;
  FILE *fimg;
  size_t npixel = header->ppmWidth * header->ppmHeight;
  uint16_t mcv = header->ppmMaxColorValue;
  channel r = malloc(npixel * sizeof(uint8_t));
  channel g = malloc(npixel * sizeof(uint8_t));
  channel b = malloc(npixel * sizeof(uint8_t));
  if (header->ppmType == PPM_MAGIC_P3) {
    fimg = fopen(filename, "r");
    if (!fimg) {
      free(r);
      free(g);
      free(b);
      return NULL;
    }

    skip_header_data_for_read(fimg);

    for (size_t i = 0; i < npixel; i++) {
      uint16_t red, green, blue;
      if (fscanf(fimg, "%hu %hu %hu", &red, &green, &blue) != 3) {
        fclose(fimg);
        free(r);
        free(g);
        free(b);
        return NULL;
      }
      r[i] = normalizeColorChannel(red, mcv);
      g[i] = normalizeColorChannel(green, mcv);
      b[i] = normalizeColorChannel(blue, mcv);
    }
    fclose(fimg);
  } else {
    fimg = fopen(filename, "rb");
    if (!fimg) {
      free(r);
      free(g);
      free(b);
      return NULL;
    }

    skip_header_data_for_read(fimg);

    size_t bytes_to_read = (mcv > 255) ? 2 : 1;

    for (int i = 0; i < npixel; i++) {
      uint16_t red, green, blue;
      if (fread(&red, bytes_to_read, 1, fimg) != 1 ||
          fread(&green, bytes_to_read, 1, fimg) != 1 ||
          fread(&blue, bytes_to_read, 1, fimg) != 1) {
        fclose(fimg);
        free(r);
        free(g);
        free(b);
        return NULL;
      }
      r[i] = normalizeColorChannel(red, mcv);
      g[i] = normalizeColorChannel(green, mcv);
      b[i] = normalizeColorChannel(blue, mcv);
    }
    fclose(fimg);
  }
  image_t *img = malloc(sizeof(image_t));
  img->width = header->ppmWidth;
  img->height = header->ppmHeight;
  img->channels[0] = r;
  img->channels[1] = g;
  img->channels[2] = b;
  return img;
}

PPMHEADER *readHeader(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp)
    return NULL;
  char magicNumber[3];
  PPMHEADER *header = malloc(sizeof(PPMHEADER));
  if (!header) {
    fclose(fp);
    return NULL;
  }
  fscanf(fp, "%2s", magicNumber);
  if (strcmp(magicNumber, "P3") == 0)
    header->ppmType = PPM_MAGIC_P3;
  else if (strcmp(magicNumber, "P6") == 0)
    header->ppmType = PPM_MAGIC_P6;
  else {
    fclose(fp);
    return NULL;
  }

  skip_comments_whitespaces(fp);
  fscanf(fp, "%u", &(header->ppmWidth));
  skip_comments_whitespaces(fp);
  fscanf(fp, "%u", &(header->ppmHeight));
  skip_comments_whitespaces(fp);
  fscanf(fp, "%hu", &(header->ppmMaxColorValue));

  fclose(fp);
  return header;
}

void skip_comments_whitespaces(FILE *fp) {
  char c;
  while ((c = fgetc(fp)) != EOF) {
    if (c == '#') { // Comment found
      while ((c = fgetc(fp)) != '\n' && c != '\r' &&
             c != EOF) { // Consuming the comment
      }
    } else if (c == ' ' || c == '\n' || c == '\r') { // Whitespaces found
      continue;
    } else { // Data found, replacing char
      ungetc(c, fp);
      return;
    }
  }
}

uint8_t normalizeColorChannel(uint16_t c, uint16_t ppmMaxColorValue) {
  return round(((double)c * 255) / ppmMaxColorValue);
}

void skip_header_data_for_read(FILE *fp) {
  char s[16];
  uint32_t w, h;
  uint16_t mcv;

  fscanf(fp, "%2s", s);
  skip_comments_whitespaces(fp);
  fscanf(fp, "%u", &w);
  skip_comments_whitespaces(fp);
  fscanf(fp, "%u", &h);
  skip_comments_whitespaces(fp);
  fscanf(fp, "%hu", &mcv);
  skip_comments_whitespaces(fp);
}
