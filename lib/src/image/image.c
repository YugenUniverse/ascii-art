#include "image/image.h"
#include "image/bmp.h"
#include "image/ppm.h"
#include "image/stb_image_handler.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

img_format get_format(const uint16_t magic) {
  switch (magic) {
  case BMP_MAGIC:
    return BMP;

  case PPM_MAGIC_P3:
  case PPM_MAGIC_P6:
    return PPM;

  default:
    return UNKNOWN;
  }
}

image_t *create_image(const char *path) {
  FILE *fimg = fopen(path, "rb");
  if (!fimg)
    return NULL;

  uint16_t magic;
  fread(&magic, sizeof(magic), 2, fimg);
  fclose(fimg);
  img_format format = get_format(magic);

  switch (format) {
  case BMP:
    return resize_img(BMP_conversion(path));
  case PPM:
    return resize_img(PPM_conversion(path));
  case UNKNOWN:
    return resize_img(loader(path));
  }
}

int computeBrightness(uint8_t r, uint8_t g, uint8_t b) {
  double Rnorm = (double)r / 255;
  double Gnorm = (double)g / 255;
  double Bnorm = (double)b / 255;

  double Rlin =
      (Rnorm <= 0.04045) ? Rnorm / 12.92 : pow(((Rnorm + 0.055) / 1.055), 2.4);
  double Glin =
      (Gnorm <= 0.04045) ? Gnorm / 12.92 : pow(((Gnorm + 0.055) / 1.055), 2.4);
  double Blin =
      (Bnorm <= 0.04045) ? Bnorm / 12.92 : pow(((Bnorm + 0.055) / 1.055), 2.4);

  double Ylin = 0.2126 * Rlin + 0.7152 * Glin + 0.0722 * Blin;

  double Ynorm = (Ylin <= 0.0031308) ? 12.92 * Ylin
                                     : (1.055 * pow(Ylin, 1.0 / 2.4) - 0.055);
  return round(10 * Ynorm);
}

char colorToChar(uint8_t r, uint8_t g, uint8_t b) {
  const char mapChar[] = {' ', '.', ',', ';', '+', '*',
                          '?', '%', '$', '#', '@'};
  return mapChar[computeBrightness(r, g, b)];
}

void print(image_t *img) {
  for (size_t i = 0; i < img->height; i++) {
    for (size_t j = 0; j < img->width; j++) {

      size_t index = i * (size_t)img->width + j;
      uint8_t r = img->channels[0][index];
      uint8_t g = img->channels[1][index];
      uint8_t b = img->channels[2][index];
      char character = colorToChar(r, g, b);
      printf("\x1b[38;2;%d;%d;%dm%c\x1b[0m", r, g, b, character);
    }
    printf("\n");
  }
}
uint8_t bilinear_interpolate_channel(uint8_t *channel, int old_width,
                                     int old_height, double src_x,
                                     double src_y) {
  // Clamp to valid range
  if (src_x < 0)
    src_x = 0;
  if (src_y < 0)
    src_y = 0;
  if (src_x >= old_width - 1)
    src_x = old_width - 1.001;
  if (src_y >= old_height - 1)
    src_y = old_height - 1.001;

  // Get integer and fractional parts
  int x1 = (int)src_x;
  int y1 = (int)src_y;
  int x2 = x1 + 1;
  int y2 = y1 + 1;

  double dx = src_x - x1;
  double dy = src_y - y1;

  // Get 4 pixels
  uint8_t Q11 = channel[y1 * old_width + x1];
  uint8_t Q21 = channel[y1 * old_width + x2];
  uint8_t Q12 = channel[y2 * old_width + x1];
  uint8_t Q22 = channel[y2 * old_width + x2];

  // Interpolate
  double result = Q11 * (1 - dx) * (1 - dy) + Q21 * dx * (1 - dy) +
                  Q12 * (1 - dx) * dy + Q22 * dx * dy;

  return (uint8_t)result;
}

image_t *resize_img(image_t *original) {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  image_t *target = malloc(sizeof(image_t));
  if (!target) {
    free_img(original);
    return NULL;
  }

  if (w.ws_col < original->width)
    target->width = w.ws_col;
  else
    target->width = original->width;

  target->height = (original->height * target->width * 0.5) / original->width;

  if (w.ws_row < target->height) {
    target->height = w.ws_row;
    target->width = 2 * target->height * original->width / original->height;
  }

  size_t npixel = target->width * target->height;
  channel new_r = malloc(npixel * sizeof(uint8_t));
  channel new_g = malloc(npixel * sizeof(uint8_t));
  channel new_b = malloc(npixel * sizeof(uint8_t));
  if (!new_r || !new_g || !new_b) {
    free_img(original);
    free(new_r);
    free(new_g);
    free(new_b);
    free_img(target);
    return NULL;
  }

  for (size_t ty = 0; ty < target->height; ty++) {
    for (size_t tx = 0; tx < target->width; tx++) {
      double src_x = tx * (double)original->width / target->width;
      double src_y = ty * (double)original->height / target->height;

      size_t new_index = ty * target->width + tx;

      new_r[new_index] =
          bilinear_interpolate_channel(original->channels[0], original->width,
                                       original->height, src_x, src_y);
      new_g[new_index] =
          bilinear_interpolate_channel(original->channels[1], original->width,
                                       original->height, src_x, src_y);
      new_b[new_index] =
          bilinear_interpolate_channel(original->channels[2], original->width,
                                       original->height, src_x, src_y);
    }
  }

  free_img(original);
  target->channels[0] = new_r;
  target->channels[1] = new_g;
  target->channels[2] = new_b;
  return target;
}

void free_img(image_t *img) {
  if (img->channels[0])
    free(img->channels[0]);
  if (img->channels[1])
    free(img->channels[1]);
  if (img->channels[2])
    free(img->channels[2]);
  free(img);
}
