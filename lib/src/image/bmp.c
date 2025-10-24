#include "image/bmp.h"
#include "image/image.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

image_t *BMP_conversion(const char *filename) {
  FILE *fimg = fopen(filename, "rb");
  if (!fimg)
    return NULL;
  BITMAPFILEHEADER header;
  BITMAPINFOHEADER info;
  fread(&header, sizeof(BITMAPFILEHEADER), 1, fimg);
  fread(&info, sizeof(BITMAPINFOHEADER), 1, fimg);

  image_t *img = malloc(sizeof(image_t));
  if (!img)
    return NULL;
  int topDown = 0;
  img->width = abs(info.biWidth);
  if (info.biHeight < 0) {
    topDown = 1;
  }
  img->height = abs(info.biHeight);

  size_t npixel = (size_t)img->height * img->width;
  channel r = malloc(npixel * sizeof(uint8_t));
  channel g = malloc(npixel * sizeof(uint8_t));
  channel b = malloc(npixel * sizeof(uint8_t));
  if (!r || !g || !b) {
    fclose(fimg);
    free_img(img);
    free(r);
    free(g);
    free(b);
    return NULL;
  }

  fseek(fimg, header.bfOffBits, SEEK_SET);
  int padding = (4 - ((img->width * 3) % 4)) % 4;
  for (int i = 0; i < img->height; i++) {
    int row = topDown ? i : (img->height - 1 - i);
    for (int j = 0; j < img->width; j++) {
      int index = row * img->width + j;
      if (fread(&b[index], sizeof(uint8_t), 1, fimg) != 1 ||
          fread(&g[index], sizeof(uint8_t), 1, fimg) != 1 ||
          fread(&r[index], sizeof(uint8_t), 1, fimg) != 1) {
        fclose(fimg);
        free_img(img);
        free(r);
        free(g);
        free(b);
        return NULL;
      }
    }
    fseek(fimg, padding, SEEK_CUR);
  }
  fclose(fimg);

  img->channels[0] = r;
  img->channels[1] = g;
  img->channels[2] = b;

  return img;
}
