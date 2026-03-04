#include "ColorSelectorComponent.hpp"
#include "../colors.hpp"

ColorSelectorComponent::ColorSelectorComponent(
    uint32_t color, bool selected, std::function<void(uint32_t color)> onSelect)
    : hovering(false), color(color), callback(onSelect), selected(false) {}

void ColorSelectorComponent::draw(uint32_t *buffer, int x, int y,
                                  int screen_width) {
  fill_box(buffer, screen_width, x, y, this->width, this->height, this->color);
  draw_empty_box(buffer, screen_width, x, y, this->width, this->height,
                 (this->hovering | this->selected)
                     ? rgb_to_uint32(128, 128, 128)
                     : rgb_to_uint32(255, 255, 255),
                 1);
}

bool ColorSelectorComponent::handleLeftMouseButtonDown(int x, int y) {
  // Change cursor to indicate the component can be clicked
  return true;
}

void ColorSelectorComponent::handleMouseEnter() { hovering = true; }

void ColorSelectorComponent::handleMouseLeave() { hovering = false; }

bool ColorSelectorComponent::handleLeftMouseButtonUp(int x, int y) {
  setSelected(true);
  callback(this->color);
  return true;
}

bool ColorSelectorComponent::isSelected() { return selected; }

void ColorSelectorComponent::setSelected(bool selcted) {
  this->selected = selcted;
}

uint32_t ColorSelectorComponent::getColor() { return color; }

int ColorSelectorComponent::getMinimumHeight(int width) { return 3; }

int ColorSelectorComponent::getMinimumWidth() { return 3; }

int ColorSelectorComponent::getMaximumHeight(int width) { return 0; }

int ColorSelectorComponent::getMaximumWidth() { return 0; }

void ColorSelectorComponent::setDimensions(int width, int height) {
  this->width = width;
  this->height = height;
}
