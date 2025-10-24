#include "image/image.h"
#include <stdio.h>

int main(int arg, char **argv) {
  image_t *img = create_image(argv[1]);
  printf("\x1b[2J\x1b[H");
  print(img);
  return 0;
}
