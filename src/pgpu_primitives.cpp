#include <pgpu/pgpu_primitives.h>

#include <cmath>

// ---------------------------------------------------------------------------
// helpers
static void plotBlended(uint32_t *fb, pgpu_resolution_t res, int col, int row,
                        uint32_t color, float alpha) noexcept {
  // bound check
  if (static_cast<uint32_t>(col) >= res.width ||
      static_cast<uint32_t>(row) >= res.height)
    return;

  uint32_t currCol = static_cast<uint32_t>(col);
  uint32_t currRow = static_cast<uint32_t>(row);

  // get bg px
  uint32_t bg = fb[currRow * res.width + currCol];

  // unpack into rgb
  uint8_t bg_r = static_cast<uint8_t>((bg >> 16) & 0xFF);
  uint8_t bg_g = static_cast<uint8_t>((bg >> 8) & 0xFF);
  uint8_t bg_b = static_cast<uint8_t>(bg & 0xFF);

  uint8_t fg_r = static_cast<uint8_t>((color >> 16) & 0xFF);
  uint8_t fg_g = static_cast<uint8_t>((color >> 8) & 0xFF);
  uint8_t fg_b = static_cast<uint8_t>(color & 0xFF);

  // blend formula is (1 - a) * bg + a * fg
  uint8_t out_r = static_cast<uint8_t>((1.0f - alpha) * bg_r + alpha * fg_r);
  uint8_t out_g = static_cast<uint8_t>((1.0f - alpha) * bg_g + alpha * fg_g);
  uint8_t out_b = static_cast<uint8_t>((1.0f - alpha) * bg_b + alpha * fg_b);

  fb[currRow * res.width + currCol] = xbgr(out_r, out_g, out_b);
}

static inline float fpart(float x) noexcept { return x - std::floor(x); }

static inline float rfpart(float x) noexcept { return 1.0f - fpart(x); }

// ---------------------------------------------------------------------------
// Circles
void drawCircle(uint32_t *fb, uint32_t stride_px, Circle c) noexcept {
  Rect bounding{
      .x = c.cx - static_cast<int>(c.radius),
      .y = c.cy - static_cast<int>(c.radius),
      .width = c.radius * 2,
      .height = c.radius * 2,
      .color = c.color,
  };

  for (int32_t y = bounding.y;
       y < bounding.y + static_cast<int32_t>(bounding.height); ++y) {
    for (int32_t x = bounding.x;
         x < bounding.x + static_cast<int32_t>(bounding.width); ++x) {
      int32_t dx = x - c.cx;
      int32_t dy = y - c.cy;
      if (dx * dx + dy * dy <= static_cast<int32_t>(c.radius * c.radius)) {
        fb[y * stride_px + x] = c.color;
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Rectangle
void drawRect(uint32_t *fb, uint32_t stride_px, Rect r,
              uint32_t color) noexcept {
  for (uint32_t row = r.y; row < r.y + r.height; ++row) {
    for (uint32_t col = r.x; col < r.x + r.width; ++col) {
      fb[row * stride_px + col] = color;
    }
  };
}

void drawRect(uint32_t *fb, pgpu_resolution_t res, int xA, int yA, int xB,
              int yB, uint32_t color) noexcept {
  int screen_w = static_cast<int>(res.width);
  int screen_h = static_cast<int>(res.height);

  int left = std::min(xA, xB);
  int right = std::max(xA, xB);
  int top = std::min(yA, yB);
  int bottom = std::max(yA, yB);

  if (left < 0)
    left = 0;
  if (right > screen_w)
    right = screen_w;
  if (top < 0)
    top = 0;
  if (bottom > screen_h)
    bottom = screen_h;

  for (int row = top; row < bottom; ++row) {
    for (int col = left; col < right; ++col) {
      fb[row * res.width + col] = color;
    }
  }
}

void drawRectStroke(uint32_t *fb, pgpu_resolution_t res, int xA, int yA, int xB,
                    int yB, uint32_t color) noexcept {
  int screen_w = static_cast<int>(res.width);
  int screen_h = static_cast<int>(res.height);

  int left = std::min(xA, xB);
  int right = std::max(xA, xB);
  int top = std::min(yA, yB);
  int bottom = std::max(yA, yB);

  if (left < 0)
    left = 0;
  if (right > screen_w)
    right = screen_w;
  if (top < 0)
    top = 0;
  if (bottom > screen_h)
    bottom = screen_h;
  if (right <= left || bottom <= top)
    return;

  for (int i = left; i < right; ++i) {
    fb[top * res.width + i] = color;
    fb[(bottom - 1) * res.width + i] = color;
  }

  for (int j = top; j < bottom; ++j) {
    fb[j * res.width + left] = color;
    fb[j * res.width + (right - 1)] = color;
  }
}

// ---------------------------------------------------------------------------
// Lines
static void lineH(uint32_t *fb, uint32_t stride_px, int x1, int x2, int y1,
                  int y2, uint32_t color, uint16_t pattern) noexcept {
  if (x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  int dx = x2 - x1;
  int dy = y2 - y1;

  int dir = (dy < 0) ? -1 : 1;
  dy *= dir;

  if (dx != 0) {
    int P = 2 * (dy - dx);
    int y = y1;
    for (int i = 0; i < dx; ++i) {
      if ((pattern >> (i % 16)) & 1)
        plot(fb, stride_px, static_cast<uint32_t>(x1 + i),
             static_cast<uint32_t>(y), color);
      if (P > 0) {
        y += dir;
        P = P - 2 * dx;
      }
      P = P + 2 * dy;
    }
  }
}

static void lineW(uint32_t *fb, uint32_t stride_px, int x1, int x2, int y1,
                  int y2, uint32_t color, uint16_t pattern) noexcept {
  if (y1 > y2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  int dx = x2 - x1;
  int dy = y2 - y1;

  int dir = (dx < 0) ? -1 : 1;
  dx *= dir;

  if (dy != 0) {
    int P = 2 * (dy - dx);
    int x = x1;
    for (int i = 0; i < dy; ++i) {
      if ((pattern >> (i % 16)) & 1)
        plot(fb, stride_px, static_cast<uint32_t>(x),
             static_cast<uint32_t>(y1 + i), color);

      if (P > 0) {
        x += dir;
        P = P - 2 * dy;
      }
      P = P + 2 * dx;
    }
  }
}

void drawLine(uint32_t *fb, uint32_t stride_px, int x1, int y1, int x2, int y2,
              uint32_t color, uint16_t pattern) noexcept {
  if (abs(x2 - x1) > abs(y2 - y1)) {
    lineH(fb, stride_px, x1, x2, y1, y2, color, pattern);
  } else {
    lineW(fb, stride_px, x1, x2, y1, y2, color, pattern);
  }
}

void drawLineAA(uint32_t *fb, pgpu_resolution_t res, int col1, int row1,
                int col2, int row2, uint32_t color) noexcept {

  // steep check
  bool steep = std::abs(row2 - row1) > std::abs(col2 - col1);
  if (steep) {
    std::swap(col1, row1);
    std::swap(col2, row2);
  }

  // left-to-right drawing
  if (col1 > col2) {
    std::swap(col1, col2);
    std::swap(row1, row2);
  }

  // compute slope gradient
  float delta_col = static_cast<float>(col2 - col1);
  float delta_row = static_cast<float>(row2 - row1);
  float gradient = (delta_col == 0.0f) ? 1.0f : (delta_row / delta_col);

  // start and end endpoints
  if (steep) {
    plotBlended(fb, res, row1, col1, color, 1.0f);
    plotBlended(fb, res, row2, col2, color, 1.0f);
  } else {
    plotBlended(fb, res, col1, row1, color, 1.0f);
    plotBlended(fb, res, col2, row2, color, 1.0f);
  }

  // col by 1 and color 2 pixels per column
  float inter_row = static_cast<float>(row1) + gradient;

  for (int col = col1 + 1; col < col2; ++col) {
    int row = static_cast<int>(std::floor(inter_row));

    if (steep) {
      plotBlended(fb, res, row, col, color, rfpart(inter_row));
      plotBlended(fb, res, row + 1, col, color, fpart(inter_row));
    } else {
      plotBlended(fb, res, col, row, color, rfpart(inter_row));
      plotBlended(fb, res, col, row + 1, color, fpart(inter_row));
    }

    inter_row += gradient;
  }
}
