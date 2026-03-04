#include "SelectorComponent.hpp"
#include "../font8x8.hpp"
#include "../utils.hpp"
#include <SDL2/SDL_keycode.h>
SelectorComponent::SelectorComponent(std::string text,
                                     std::function<void(bool)> callback,
                                     bool selected, bool allowUnselecting,
                                     uint32_t selectedColor) {
  this->hovering = false;
  this->text = text;
  this->callback = callback;
  this->selected = selected;
  this->allowUnselecting = allowUnselecting;
  this->selectedColor = selectedColor;
}

void SelectorComponent::draw(uint32_t *buffer, int x, int y, int screen_width) {
  uint32_t color = (this->hovering || this->selected)
                       ? this->selectedColor
                       : rgb_to_uint32(255, 255, 255);
  draw_empty_box(buffer, screen_width, x, y, this->width, this->height, color,
                 1);
  RenderText8x8(buffer, screen_width, x + xPadding, y + yPadding, this->text,
                color, this->textScale);
}

void SelectorComponent::handleMouseEnter() { this->hovering = true; }

void SelectorComponent::handleMouseLeave() { this->hovering = false; }

bool SelectorComponent::handleLeftMouseButtonDown(int x, int y) { return true; }

bool SelectorComponent::handleLeftMouseButtonUp(int x, int y) {
  if (!this->selected || this->allowUnselecting) {
    this->selected = !this->selected;

    this->hovering = this->selected;
    this->callback(this->selected);
  }
  return true;
}

void SelectorComponent::setSelected(bool selected) {
  this->selected = selected;
}

bool SelectorComponent::isSelected() { return this->selected; }

int SelectorComponent::getMinimumWidth() {
  return textWidth(this->text, 1) + 2;
}

int SelectorComponent::getMinimumHeight(int width) { return 4; }

int SelectorComponent::getMaximumHeight(int width) { return 0; }

int SelectorComponent::getMaximumWidth() { return 0; }
void SelectorComponent::setDimensions(int w, int h) {
  this->width = w;
  this->height = h;
  this->textScale = optimalScale(this->text, w - 2, h - 2);
  this->xPadding = (w - textWidth(this->text, this->textScale)) / 2;
  this->yPadding = (h - 8 * textScale) / 2;
}