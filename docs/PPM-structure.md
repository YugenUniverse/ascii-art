# PPM Format Structure

A PPM file is divided into two main parts:
1. [PPM HEADER](#ppm-header) — general information about the file.
2. [DATA](#data)

All the data are encoded in little endian.

---

## PPM Header

| Size(bytes) | Fields | Description |
| -------------- | --------------- | --------------- |
| 2 | ppmType | File Type, must be "P3" = 0x3350 or "P6" = 0x3650 |
| Not Fixed | ppmWidth | Image width in pixel  |
| Not Fixed | ppmHeight | Image height in pixel |
| 8-16 | ppmMaxColorValue | Maximum value for each channel of a color. |

The header can be as full as you want of comments, perceived by '#'.

---

## Data

If it's a "P3" ppm file data are encoded in ASCII format, and the channel are divided by " ".
So red will be `'2''5''5'' ''0'' ''0'`.

In the "P6" version data are in binary so the same red will be `111111110000000000000000`.



