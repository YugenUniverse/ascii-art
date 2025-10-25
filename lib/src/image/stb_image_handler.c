#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image_handler.h"
#include "image/image.h"
#include "stb_image.h"
#include <stdint.h>

image_t *loader(const char *filename) {
  image_t *img = malloc(sizeof(image_t));
  if (!img)
    return NULL;
  int width, height;
  int channels;

  unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);
  if (!data) {
    printf("stdi_load error");
    free_img(img);
    return NULL;
  }
  img->height = (uint32_t)height;
  img->width = (uint32_t)width;

  size_t npixel = (size_t)img->height * (size_t)img->width;
  channel red = malloc(sizeof(uint8_t) * npixel);
  channel green = malloc(sizeof(uint8_t) * npixel);
  channel blue = malloc(sizeof(uint8_t) * npixel);
  if (!red || !green || !blue) {
    printf("channels allocation error");
    free(red);
    free(green);
    free(blue);
    free_img(img);
    stbi_image_free(data);
    return NULL;
  }

  for (size_t i = 0, j = 0; j < npixel; i += 3, j++) {
    red[j] = data[i];
    green[j] = data[i + 1];
    blue[j] = data[i + 2];
  }
  stbi_image_free(data);

  img->channels[0] = red;
  img->channels[1] = green;
  img->channels[2] = blue;
  return img;
}
