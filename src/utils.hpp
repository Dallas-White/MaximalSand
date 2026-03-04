#include <cstdlib>
#include <stdint.h>

#ifndef UTILS
#define UTILS

struct XorShift32 {
  uint32_t state = 0x6d2b79f5u;

  inline uint32_t next() {
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state = x;
  }
};
inline constexpr uint32_t rgb_to_uint32(int r, int g, int b) {
  return (0xFFU << 24) | (r << 16) | (g << 8) | b;
}

inline constexpr void uint32_to_rgb(uint32_t argb, uint8_t *r, uint8_t *g,
                                    uint8_t *b) {
  *r = (argb >> 16) & 0xFF;
  *g = (argb >> 8) & 0xFF;
  *b = argb & 0xFF;
}

inline void draw_empty_box(uint32_t *buffer, int screen_width, int x, int y,
                           int width, int height, uint32_t color,
                           int thickness = 1) {
  for (int i = 0; i < thickness; i++) {
    for (int dx = 0; dx < width; dx++) {
      int px = x + dx;
      int py = y + i;
      if (px >= 0 && px < screen_width) {
        buffer[py * screen_width + px] = color;
      }
    }

    for (int dx = 0; dx < width; dx++) {
      int px = x + dx;
      int py = y + height - 1 - i;
      buffer[py * screen_width + px] = color;
    }

    for (int dy = 1; dy < height - 1; dy++) {
      int px = x + i;
      int py = y + dy;
      buffer[py * screen_width + px] = color;
    }

    for (int dy = 1; dy < height - 1; dy++) {
      int px = x + width - 1 - i;
      int py = y + dy;
      buffer[py * screen_width + px] = color;
    }
  }
}

inline void fill_box(uint32_t *buffer, int screen_width, int x, int y,
                     int width, int height, uint32_t color) {

  for (int dy = 0; dy < height; ++dy) {
    uint32_t row = (dy + y) * screen_width;
    for (int dx = 0; dx <= width; dx++) {
      buffer[row + dx + x] = color;
    }
  }
}

inline void calculateCenteredItem(int x, int y, int width, int height,
                                  int itemWidth, int itemHeight, int &itemX,
                                  int &itemY) {
  itemX = x + std::abs(width - itemWidth) / 2;
  itemY = y + std::abs(height - itemHeight) / 2;
}

#endif