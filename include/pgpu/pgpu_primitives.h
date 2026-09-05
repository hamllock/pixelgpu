#pragma once

#include "pgpu_types.h"

void drawRect(uint32_t *fb, pgpu_resolution_t res, int xA, int yA, int xB,
              int yB, uint32_t color) noexcept;
void drawRectStroke(uint32_t *fb, pgpu_resolution_t res, int xA, int yA, int xB,
                    int yB, uint32_t color) noexcept;
void drawCircle(uint32_t *fb, uint32_t stride_px, Circle c) noexcept;
void drawLine(uint32_t *fb, pgpu_resolution_t res, int x1, int y1, int x2,
              int y2, uint32_t color, uint16_t pattern = 0xFFFF,
              uint32_t thickness = 1) noexcept;
void drawLineAA(uint32_t *fb, pgpu_resolution_t res, int x1, int y1, int x2,
                int y2, uint32_t color) noexcept;
