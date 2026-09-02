#include <cstdio>
#include <pgpu/pgpu.h>

int main() {
  const pgpu_resolution_t res =
      pgpu_resolution_from_preset(PGPU_RES_VGA); // 640x480

  pgpu_display_t display = pgpu_display_create(
      res, nullptr, 60, "PixelGPU Engine", nullptr, nullptr);
  if (!display) {
    std::fprintf(stderr, "Failed to create display: %s\n",
                 pgpu_last_error_string());
    return 1;
  }

  uint32_t *fb = static_cast<uint32_t *>(pgpu_display_get_framebuffer(display));

  // Clear to black
  for (uint32_t i = 0; i < res.width * res.height; ++i) {
    fb[i] = 0xFF000000;
  }

  while (pgpu_display_is_alive(display)) {
    pgpu_wait_vsync(display, 100);
    pgpu_display_present_now(display);
  }

  pgpu_display_destroy(display);
  return 0;
}
