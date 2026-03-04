#include "ButtonComponent.hpp"
#include "../font8x8.hpp"
#include "../utils.hpp"
#include <cassert>
ButtonComponent::ButtonComponent(std::string text, bool showBorder,
                                 uint8_t foregroundR, uint8_t foregroundG,
                                 uint8_t foregroundB,
                                 std::function<void()> callback) {
  this->foregroundColor = rgb_to_uint32(foregroundR, foregroundG, foregroundB);
  this->dimmedColor =
      rgb_to_uint32(foregroundR / 2, foregroundG / 2, foregroundB / 2);
  this->text = text;
  this->showBorder = showBorder;
  this->callback = callback;
  this->hovering = false;
}

void ButtonComponent::draw(uint32_t *buffer, int x, int y, int screen_width) {
  if (this->showBorder) {
    draw_empty_box(buffer, screen_width, x, y,
                   textWidth(this->text, this->textScale) +
                       this->horizontalPadding * 2,
                   this->textScale * 8 + this->verticalPadding * 2,
                   this->hovering ? dimmedColor : foregroundColor);
  }
  RenderText8x8(buffer, screen_width, x + horizontalPadding,
                y + verticalPadding, this->text,
                this->hovering ? dimmedColor : foregroundColor,
                this->textScale);
}

bool ButtonComponent::handleLeftMouseButtonUp(int x, int y) {
  this->callback();
  return true;
}

bool ButtonComponent::handleLeftMouseButtonDown(int x, int y) { return true; }

void ButtonComponent::handleMouseLeave() { this->hovering = false; }

void ButtonComponent::handleMouseEnter() { this->hovering = true; }

int ButtonComponent::getMinimumWidth() { return textWidth(this->text, 1) + 8; }

int ButtonComponent::getMinimumHeight(int width) { return 12; }

int ButtonComponent::getMaximumHeight(int width) { return 0; }

int ButtonComponent::getMaximumWidth() { return 0; }

void ButtonComponent::setDimensions(int w, int h) {
  assert(w >= this->getMinimumWidth());
  this->textScale = std::max(1, optimalScale(this->text, w - 4, h - 4));
  this->horizontalPadding = (w - textWidth(this->text, this->textScale)) / 2;
  this->verticalPadding = (h - 8 * textScale) / 2;
}