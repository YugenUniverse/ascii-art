# BMP Format Structure

A BMP file is divided into four main parts:
1. [BITMAP FILE HEADER](#bitmap-file-header) — general information about the file.
2. [DIB HEADER](#dib-header) (BITMAP INFO HEADER) — details about the image itself (width, height, color depth, etc.).
3. [COLOR TABLE](#color-table) (**optional**) — defines the colors used (for indexed color images).
4. [PIXEL ARRAY](#pixel-array) (Bitmap Data) — actual pixel data.

All the data are encoded in little endian.

---

## Bitmap File Header
Fixed Size 14 Bytes

| Size(bytes) | Fields | Description |
| -------------- | --------------- | --------------- |
| 2 | bfType | File Type, must be "BM" = 0x4d42 |
| 4 | bfSize | Size of the entire BMP file in bytes |
| 2 | bfReserved1 | Reserved; must be 0 |
| 2 | bfReserved2 | Reserved; must be 0 |
| 4 | bfOffBits | Offset from start of file to the beginning of the pixel data |

---

## DIB Header
Variable Size
There are several versions of the DIB Header; the most common is **BITMAPINFOHEADER** *(40 bytes)*

| Size(bytes) | Fields | Description |
| --------------- | --------------- | --------------- |
| 4 | biSize | Size of this DIB header |
| 4 | biWidth | Image width in pixel |
| 4 | biHeight | Image height in pixel. (if negative -> top-down bitmap) |
| 2 | biPlanes | Number of color planes; must be 1 |
| 2 | biBitCount | Bits per pixel (1, 4, 8, 16, 24, or 32) |
| 4 | biCompression | Compression method (0 = BI_RGB = none) |
| 4 | biSizeImage | Size of pixel data (may be 0 if uncompressed) |
| 4 | biXPelsPerMeter | Horizontal resolution (pixels per meter) |
| 4 | biYPelsPerMeter | Vertical resolution (pixels per meter) |
| 4 | biClrUsed | Number of colors in the palette (0 = default) |
| 4 | biClrImportant | Number of important colors (0 = all) |

---

## Color Table
Present only if biBitCount ≤ 8 (1, 4, or 8 bits per pixel).
Each entry is 4 bytes:

| Fields | Bytes | Description |
| --------------- | --------------- | --------------- |
| Blue | 1 | Blue intensity (0-255) |
| Green | 1 | Green intensity (0-255) |
| Red | 1 | Red intensity (0-255) |
| Reserved | 1 | Usually 0 |


Number of entries = 2^biBitCount or biClrUsed

---

## Pixel Array
Begins at the offset given by bfOffBits.
Each row of pixels is padded to a multiple of 4 bytes.

Pixel order:
- Bottom-up (default): last row in file = top of image.
- Top-down if biHeight is negative.

Pixel format depends on biBitCount:
- 24-bit: BGR (Blue, Green, Red)
- 32-bit: BGRA (Blue, Green, Red, Alpha)
- 8-bit or less: Indices into the color table
