#pragma once

#include "pgpu_types.h"

void blockCopy(uint32_t *fb, pgpu_resolution_t res, int sX, int sY, int dX,
               int dY, uint32_t width, uint32_t height);
void scaledCopy(uint32_t *fb, pgpu_resolution_t res, int sX, int sY, int dX,
                int dY, uint32_t sW, uint32_t sH, uint32_t dW, uint32_t dH);
