#include "ColorComponent.hpp"
#include "../utils.hpp"
#include <cassert>
ColorComponent::ColorComponent(Uint32 color) { this->color = color; }

void ColorComponent::draw(uint32_t *buffer, int x, int y, int screen_width) {
  fill_box(buffer, screen_width, x, y, this->width, this->height, color);
}

int ColorComponent::getMinimumHeight(int width) { return 1; }

int ColorComponent::getMinimumWidth() { return 1; }

int ColorComponent::getMaximumHeight(int width) { return 0; }

int ColorComponent::getMaximumWidth() { return 0; }

void ColorComponent::setDimensions(int width, int height) {
  this->width = width;
  this->height = height;
}
