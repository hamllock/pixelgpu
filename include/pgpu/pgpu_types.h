#pragma once

#include <pgpu/pgpu.h>

#include <cstdint>

struct Rect {
  int32_t x;
  int32_t y;
  uint32_t width;
  uint32_t height;
  uint32_t color;
};

struct Circle {
  int32_t cx;
  int32_t cy;
  uint32_t radius;
  uint32_t color;
};

// ---------------------------------------------------------------------------
// XBGR8888 pixel packing helper
static inline uint32_t xbgr(uint8_t r, uint8_t g, uint8_t b) noexcept {
  // Byte layout in memory: [B][G][R][X]
  // As a uint32_t on little-endian: X=byte3, R=byte2, G=byte1, B=byte0
  return (static_cast<uint32_t>(0xFF) << 24) // X = 0xFF (unused)
         | (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) |
         (static_cast<uint32_t>(b));
}

static inline void plot(uint32_t *fb, uint32_t stride_px, uint32_t x,
                        uint32_t y, uint32_t color) noexcept {
  fb[y * stride_px + x] = color;
}
