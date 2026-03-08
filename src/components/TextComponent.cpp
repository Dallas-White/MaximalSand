#include "TextComponent.hpp"
#include "../font8x8.hpp"

TextComponent::TextComponent(std::vector<std::string> text, uint32_t color) {
  this->text = text;
  this->color = color;
  this->xPadding.resize(text.size());
}

void TextComponent::draw(uint32_t *buffer, int x, int y, int screen_width) {

  for (int i = 0; i < this->text.size(); i++) {
    RenderText8x8(buffer, screen_width, x + this->xPadding[i],
                  y + i * this->scale * 9, this->text[i], this->color,
                  this->scale);
  }
}

int TextComponent::getMinimumHeight(int width) {
  return (this->text.size() * 12);
}

int TextComponent::getMinimumWidth() {
  int minimumWidth = 0;
  for (std::string &line : this->text) {
    minimumWidth = std::max(minimumWidth, textWidth(line, 1));
  }
  return minimumWidth + 2;
}

int TextComponent::getMaximumHeight(int width) { return 0; }

int TextComponent::getMaximumWidth() { return 0; }

void TextComponent::setDimensions(int width, int height) {
  int textCellheight = (height - this->text.size() - 4) / this->text.size();
  this->scale = INT_MAX;
  for (std::string line : this->text) {
    this->scale =
        std::min(optimalScale(line, width - 4, textCellheight), this->scale);
  }
  this->yPadding = (height - this->scale * 8) / 2;
  for (int i = 0; i < this->text.size(); i++) {
    this->xPadding[i] = (width - textWidth(this->text[i], this->scale)) / 2;
  }
}