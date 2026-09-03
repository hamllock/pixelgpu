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

// dotted and dashed demo
static void scene_lines(uint32_t *fb, pgpu_resolution_t res, uint64_t tick) {
  drawRect(fb, res, 0, 0, res.width, res.height, xbgr(15, 18, 25));

  // solid
  drawLine(fb, res.width, 50, 80, 590, 80, xbgr(255, 255, 255));
  // dotted
  drawLine(fb, res.width, 50, 160, 590, 160, xbgr(100, 255, 100), 0xAAAA);
  // dashed
  drawLine(fb, res.width, 50, 240, 590, 240, xbgr(255, 200, 50), 0xF0F0);
  // long dash
  drawLine(fb, res.width, 50, 320, 590, 320, xbgr(255, 100, 255), 0xFF00);
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
    drawLine(fb, res.width, 160, 240, end_col, end_row, xbgr(255, 80, 80));
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

int main() {
  const pgpu_resolution_t res = pgpu_resolution_from_preset(PGPU_RES_VGA);
  pgpu_display_t display =
      pgpu_display_create(res, nullptr, 60, "pgpu demo", nullptr, nullptr);
  if (!display)
    return 1;

  uint32_t *fb = static_cast<uint32_t *>(pgpu_display_get_framebuffer(display));

  void (*scenes[])(uint32_t *, pgpu_resolution_t, uint64_t) = {
      // scene_primitives,
      // scene_lines,
      scene_antialiasing,
      // scene_blitter,
  };
  const int num_scenes = sizeof(scenes) / sizeof(scenes[0]);

  uint64_t frame = 0;
  while (pgpu_display_is_alive(display)) {
    int current_scene = (frame / 60) % num_scenes;

    scenes[current_scene](fb, res, frame);

    pgpu_wait_vsync(display, 100);
    pgpu_display_present_now(display);
    ++frame;
  }

  pgpu_display_destroy(display);
  return 0;
}
