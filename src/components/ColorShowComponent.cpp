#include "ColorShowComponent.hpp"
#include "../utils.hpp"
#include "../windows/PaletteWindow.hpp"
#include <iostream>
const Uint8 OUTLINE_PIXELS = 1;
ColorShowComponent::ColorShowComponent(SandComponent *sw, WindowManager *wm,
                                       int padding) {
  this->sw = sw;
  this->padding = padding;
  this->hovering = false;
  this->wm = wm;
}

void ColorShowComponent::draw(uint32_t *buffer, int x, int y,
                              int screen_width) {
  fill_box(buffer, screen_width, x, y, this->width, this->height,
           this->sw->getSandColor());
  draw_empty_box(buffer, screen_width, x, y, this->width, this->height,
                 this->hovering ? rgb_to_uint32(128, 128, 128)
                                : rgb_to_uint32(255, 255, 255),
                 2);
}

void ColorShowComponent::handleMouseEnter() { this->hovering = true; }

void ColorShowComponent::handleMouseLeave() { this->hovering = false; }

bool ColorShowComponent::handleLeftMouseButtonDown(int x, int y) {
  return true;
}

bool ColorShowComponent::handleLeftMouseButtonUp(int x, int y) {
  if (this->wm->isWindowOpen<PaletteWindow>())
    return true;
  std::cout << this->wm->getHeight() << std::endl;
  this->wm->addWindow(new PaletteWindow(this->sw, this->wm), 0, this->height,
                      std::min(this->wm->getWidth(), 300),
                      std::min(this->wm->getHeight() - this->height, 400));
  return true;
}

int ColorShowComponent::getMinimumHeight(int width) { return 10; }

int ColorShowComponent::getMinimumWidth() { return 10; }

int ColorShowComponent::getMaximumWidth() { return 0; }

int ColorShowComponent::getMaximumHeight(int width) { return 0; }

void ColorShowComponent::setDimensions(int width, int height) {
  this->width = width;
  this->height = height;
}
