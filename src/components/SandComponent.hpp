#include "../sand.hpp"
#include "../utils.hpp"
#include "windowComponent.hpp"
#include <SDL2/SDL_keycode.h>
#include <cstdint>
#ifndef SANDWINDOW
#define SANDWINDOW
class SandComponent : public windowComponent {
private:
  bool drawMouse = true;
  uint32_t sandColor;
  Sand sandSim;
  XorShift32 rng;
  uint8_t brushSize = 20;
  int width;
  int height;
  bool placing = false;
  bool deleting = false;
  bool shuffle = true;
  bool eraseMode = false;
  bool showBrushSize = false;

public:
  SandComponent();
  void handleKeyPress(SDL_Keysym code) override;
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  void setDimensions(int width, int height) override;
  bool handleLeftMouseButtonDown(int x, int y) override;
  bool handleLeftMouseButtonUp(int x, int y) override;
  void handleMouseLeave() override;
  void handleMouseEnter() override;
  bool drawsMouse() override;
  bool handleRightMouseButtonDown(int x, int y) override;
  bool handleRightMouseButtonUp(int x, int y) override;
  void onMouseLeave();
  void onMouseEnter();
  void setShuffle(bool shuffle);
  bool getShuffle();
  void setSandColor(uint32_t color);
  void InvertGravity();
  uint32_t getSandColor();
  uint8_t getBrushSize();
  void setBrushSize(uint8_t brushSize);
  void pickNewColor();
  int getWidth() override;
  int getHeight() override;
  int getMaximumHeight(int width) override;
  int getMaximumWidth() override;
  int getMinimumHeight(int width) override;
  int getMinimumWidth() override;
  void setEraseMode(bool eraseMode);
  void clear();
};
#endif