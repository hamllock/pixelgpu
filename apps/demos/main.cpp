#include "pgpu/pgpu_primitives.h"
#include <cmath>
#include <cstdio>
#include <pgpu/pgpu.h>
#include <pgpu/pgpu_gl.h>

// primitive shapes demo
static void scene_primitives(uint32_t *fb, pgpu_resolution_t res,
                             uint64_t tick) {
  drawRect(fb, res, 0, 0, res.width, res.height, xbgr(15, 18, 25));

  // normal rect
  drawRect(fb, res, 50, 50, 200, 200, xbgr(0, 150, 255));
  // rect with stroke
  drawRectStroke(fb, res, 250, 50, 400, 200, xbgr(255, 200, 0));
  // circle
  drawCircle(
      fb, res.width,
      Circle{.cx = 520, .cy = 125, .radius = 80, .color = xbgr(255, 50, 100)});
}
// starburst demo
static void scene_antialiasing(uint32_t *fb, pgpu_resolution_t res,
                               uint64_t tick) {
  drawRect(fb, res, 0, 0, res.width, res.height, xbgr(15, 18, 25));

  // Bresenham
  for (int angle = 0; angle < 360; angle += 15) {
    float spin = static_cast<float>(tick) * 0.005f;
    float rad = static_cast<float>(angle) * (3.14159265f / 180.0f) + spin;
    int end_col = 160 + static_cast<int>(std::cos(rad) * 110.0f);
    int end_row = 240 + static_cast<int>(std::sin(rad) * 110.0f);
    drawLine(fb, res, 160, 240, end_col, end_row, xbgr(255, 80, 80));
  }

  // Xiaolin Wu
  for (int angle = 0; angle < 360; angle += 15) {
    float spin = static_cast<float>(tick) * 0.005f;
    float rad = static_cast<float>(angle) * (3.14159265f / 180.0f) + spin;
    int end_col = 480 + static_cast<int>(std::cos(rad) * 110.0f);
    int end_row = 240 + static_cast<int>(std::sin(rad) * 110.0f);
    drawLineAA(fb, res, 480, 240, end_col, end_row, xbgr(80, 220, 255));
  }
}

// blitter demo
static void scene_blitter(uint32_t *fb, pgpu_resolution_t res, uint64_t tick) {
  drawRect(fb, res, 0, 0, res.width, res.height, xbgr(15, 18, 25));

  // src
  drawRect(fb, res, 50, 50, 114, 114, xbgr(255, 100, 50));
  drawCircle(
      fb, res.width,
      Circle{.cx = 82, .cy = 82, .radius = 20, .color = xbgr(255, 255, 255)});

  // 1x copy
  blockCopy(fb, res, 50, 50, 200, 50, 64, 64);

  // 3x copy
  scaledCopy(fb, res, 50, 50, 320, 50, 64, 64, 192, 192);
}

// lines demo
static void scene_thickness_and_patterns(uint32_t *fb, pgpu_resolution_t res,
                                         uint64_t tick) {
  drawRect(fb, res, 0, 0, res.width, res.height, xbgr(15, 18, 25));

  uint32_t thicknesses[] = {1, 3, 5, 7, 9};
  int base_row = 40;
  for (size_t i = 0; i < 5; ++i) {
    int row = base_row + static_cast<int>(i) * 35;
    drawLine(fb, res, 40, row, 260, row, xbgr(0, 200, 255), 0xFFFF,
             thicknesses[i]);
  }

  // patterns
  drawLine(fb, res, 40, 240, 260, 240, xbgr(255, 255, 255), 0xFFFF,
           3); // solid
  drawLine(fb, res, 40, 280, 260, 280, xbgr(100, 255, 100), 0xAAAA,
           3); // dotted
  drawLine(fb, res, 40, 320, 260, 320, xbgr(255, 200, 50), 0xCCCC,
           3); // dashed
  drawLine(fb, res, 40, 360, 260, 360, xbgr(255, 100, 255), 0xF0F0,
           3); // wide dash
  drawLine(fb, res, 40, 400, 260, 400, xbgr(255, 80, 80), 0xFF00, 3);

  for (size_t i = 0; i < 4; ++i) {
    int col = 300 + static_cast<int>(i) * 30;
    drawLine(fb, res, col, 40, col, 200, xbgr(255, 180, 50), 0xFFFF,
             thicknesses[i]);
  }

  // Rotating spokes
  int center_col = 500;
  int center_row = 300;
  float spin = static_cast<float>(tick) * 0.02f;
  float radius = 100.0f;

  for (int angle = 0; angle < 360; angle += 45) {
    float rad = static_cast<float>(angle) * (3.14159265f / 180.0f) + spin;
    int end_col = center_col + static_cast<int>(std::cos(rad) * radius);
    int end_row = center_row + static_cast<int>(std::sin(rad) * radius);
    drawLine(fb, res, center_col, center_row, end_col, end_row,
             xbgr(255, 80, 120), 0xFFFF, 6);
  }
}

int main() {
  const pgpu_resolution_t res = pgpu_resolution_from_preset(PGPU_RES_VGA);
  pgpu_display_t display =
      pgpu_display_create(res, nullptr, 60, "pixelgpu", nullptr, nullptr);
  if (!display)
    return 1;

  uint32_t *fb = static_cast<uint32_t *>(pgpu_display_get_framebuffer(display));

  uint64_t frame = 0;
  while (pgpu_display_is_alive(display)) {
    scene_thickness_and_patterns(fb, res, frame);

    pgpu_wait_vsync(display, 100);
    pgpu_display_present_now(display);
    ++frame;
  }

  pgpu_display_destroy(display);
  return 0;
}
