#include <pgpu/pgpu_blitter.h>

#include <cstring>

// ---------------------------------------------------------------------------
// Blitting
void blockCopy(uint32_t *fb, pgpu_resolution_t res, int sX, int sY, int dX,
               int dY, uint32_t width, uint32_t height) {

  if (dY > sY) {
    for (int row = static_cast<int>(height) - 1; row >= 0; --row) {
      uint32_t *src_row = &fb[(sY + row) * res.width + sX];
      uint32_t *dst_row = &fb[(dY + row) * res.width + dX];
      std::memmove(dst_row, src_row, width * sizeof(uint32_t));
    }
  } else {
    for (int row = 0; row < static_cast<int>(height); ++row) {
      uint32_t *src_row = &fb[(sY + row) * res.width + sX];
      uint32_t *dst_row = &fb[(dY + row) * res.width + dX];
      std::memmove(dst_row, src_row, width * sizeof(uint32_t));
    }
  }
}

void scaledCopy(uint32_t *fb, pgpu_resolution_t res, int sX, int sY, int dX,
                int dY, uint32_t sW, uint32_t sH, uint32_t dW, uint32_t dH) {

  int stride = static_cast<int>(res.width);
  int src_w = static_cast<int>(sW);
  int src_h = static_cast<int>(sH);
  int dst_w = static_cast<int>(dW);
  int dst_h = static_cast<int>(dH);

  if (dY > sY) {
    for (int row = dst_h - 1; row >= 0; --row) {
      int src_row = (row * src_h) / dst_h;
      for (int col = dst_w - 1; col >= 0; --col) {
        int src_col = (col * src_w) / dst_w;
        fb[(dY + row) * stride + (dX + col)] =
            fb[(sY + src_row) * stride + (sX + src_col)];
      }
    }
  } else {
    for (int row = 0; row < dst_h; ++row) {
      int src_row = (row * src_h) / dst_h;
      for (int col = 0; col < dst_w; ++col) {
        int src_col = (col * src_w) / dst_w;
        fb[(dY + row) * stride + (dX + col)] =
            fb[(sY + src_row) * stride + (sX + src_col)];
      }
    }
  }
}
