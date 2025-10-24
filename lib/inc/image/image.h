#ifndef IMAGE_H
#define IMAGE_H
#include <stdint.h>

#define BMP_MAGIC 0x4d42

#define PPM_MAGIC_P3 0x3350
#define PPM_MAGIC_P6 0x3650

typedef uint8_t *channel;

typedef struct {
  uint32_t width;
  uint32_t height;

  channel channels[3];
} image_t;

typedef enum { BMP = 0, PPM, UNKNOWN } img_format;

image_t *create_image(const char *path);
img_format get_format(const uint16_t magic);
void free_img(image_t *img);

void print(image_t *img);
char colorToChar(const uint8_t r, const uint8_t g, const uint8_t b);
int computeBrightness(const uint8_t r, const uint8_t g, const uint8_t b);

image_t *resize_img(image_t *original);
uint8_t bilinear_interpolate_channel(uint8_t *channel, const int old_width,
                                     const int old_height, double src_x,
                                     double src_y);
#endif // !IMAGE_H
