#include "Window.hpp"
#include "../utils.hpp"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <climits>
#include <cstdint>
#include <vector>

bool Window::inBound(AddedComponent &wc, int x, int y) {
  return x >= wc.x && x < wc.x + wc.width && y >= wc.y && y < wc.height + wc.y;
}

AddedComponent *Window::findMouseComponent(int x, int y) {
  for (int i = this->components.size() - 1; i >= 0; i--) {
    AddedComponent &wc = this->components[i];
    if (this->inBound(wc, x, y)) {
      return &wc;
    }
  }
  return NULL;
}
Window::Window(int gridWidth, int gridHeight, int xPadding, int yPadding,
               bool drawBorder, WindowManager *wm) {
  this->gridWidth = gridWidth;
  this->gridHeight = gridHeight;
  this->drawBorder = drawBorder;
  this->xPadding = xPadding;
  this->yPadding = yPadding;
  this->wm = wm;
  this->cellWidth.resize(gridWidth);
  this->cellHeight.resize(gridHeight);
}

Window::~Window() {
  for (AddedComponent &wc : this->components) {
    delete wc.component;
  }
}

Window *Window::setTemporary() {
  this->temporary = true;
  return this;
}

void Window::handleKeyPress(SDL_Keysym key) {
  for (AddedComponent &wc : this->components) {
    wc.component->handleKeyPress(key);
  }
}

void Window::draw(uint32_t *buffer, int x, int y, int screen_width) {

  if (this->drawBackground) {

    for (int dy = y; dy < this->height + y; dy++) {
      int currentRow = dy * screen_width;
      for (int dx = x; dx < this->width + x; dx++) {
        uint32_t pixel = buffer[currentRow + dx];
        if (pixel != 0) {
          uint8_t r;
          uint8_t g;
          uint8_t b;
          uint32_to_rgb(pixel, &r, &g, &b);
          r = 10 + r / 2;
          g = 10 + g / 2;
          b = 10 + b / 2;

          buffer[currentRow + dx] = rgb_to_uint32(r, g, b);
        } else {
          buffer[currentRow + dx] = rgb_to_uint32(10, 10, 10);
        }
      }
    }
  }
  if (this->drawBorder) {
    draw_empty_box(buffer, screen_width, x, y, this->width, this->height,
                   rgb_to_uint32(255, 255, 255));
  }
  for (AddedComponent &c : this->components) {
    c.component->draw(buffer, c.x + x, c.y + y, screen_width);
  }
  if (this->drawMouse && (!this->mouseComponent ||
                          !this->mouseComponent->component->drawsMouse())) {
    for (int dx = -2; dx <= 2; dx++) {
      if (this->mouseX + x + dx >= 0) {
        for (int dy = -2; dy <= 2; dy++) {
          if (this->mouseY + y + dy >= 0 &&
              this->mouseY + y + dy < y + this->height) {

            buffer[(this->mouseX + x + dx) +
                   (this->mouseY + y + dy) * screen_width] =
                rgb_to_uint32(255, 255, 255);
          }
        }
      }
    }
  }
}

bool Window::handleRightMouseButtonDown(int x, int y) {
  for (int i = this->components.size() - 1; i >= 0; i--) {
    AddedComponent &c = this->components.at(i);
    if (inBound(c, x, y)) {
      this->activeComponent = &c;
      if (c.component->handleRightMouseButtonDown(x, y)) {
        return true;
      }
    }
  }
  return false;
}

bool Window::handleRightMouseButtonUp(int x, int y) {
  for (int i = this->components.size() - 1; i > 0; i--) {
    AddedComponent &c = this->components.at(i);
    if (inBound(c, x, y)) {
      if (c.component->handleRightMouseButtonDown(x, y)) {
        c.component->handleRightMouseButtonUp(x, y);
        return true;
      }
    }
  }
  return false;
}

bool Window::handleLeftMouseButtonDown(int x, int y) {
  for (int i = this->components.size() - 1; i > 0; i--) {
    AddedComponent &c = this->components.at(i);
    if (inBound(c, x, y)) {
      if (c.component->handleLeftMouseButtonDown(x, y)) {
        this->activeComponent = &c;
        return true;
      }
    }
  }
  return false;
}

bool Window::handleLeftMouseButtonUp(int x, int y) {
  for (int i = this->components.size() - 1; i > 0; i--) {
    AddedComponent &c = this->components.at(i);
    if (inBound(c, x, y)) {
      c.component->handleLeftMouseButtonUp(x, y);
      return true;
    }
  }
  return false;
}

void Window::handleMouseEnter() { this->drawMouse = true; }

void Window::handleMouseLeave() {
  if (this->temporary)
    wm->removeWindow(this);
  if (this->mouseComponent)
    this->mouseComponent->component->handleMouseLeave();
  this->mouseComponent = NULL;
  this->drawMouse = false;
}

void Window::setMousePosition(int x, int y) {

  this->drawMouse = true;
  AddedComponent *newMouseComponent = this->findMouseComponent(x, y);
  if (this->mouseComponent != newMouseComponent) {
    if (this->mouseComponent != NULL)
      this->mouseComponent->component->handleMouseLeave();
    if (newMouseComponent != NULL)
      newMouseComponent->component->handleMouseEnter();
    this->mouseComponent = newMouseComponent;
  }
  this->mouseX = x;
  this->mouseY = y;
  if (this->mouseComponent) {
    this->mouseComponent->component->setMousePosition(x, y);
  }
}

void Window::addComponent(windowComponent *component, int gridX, int gridY,
                          int gridWidth, int gridHeight) {
  AddedComponent ac;
  ac.component = component;
  ac.gridX = gridX;
  ac.gridY = gridY;
  ac.gridWidth = gridWidth;
  ac.gridHeight = gridHeight;
  this->components.push_back(ac);
}

void Window::removeComponent(windowComponent *component) {
  for (int x = 0; x < this->components.size(); x++) {
    if (component == this->components[x].component) {
      this->components[x] = this->components[this->components.size() - 1];
      this->components.pop_back();
      return;
    }
  }
}

std::vector<windowComponent *> Window::getComponents() {
  std::vector<windowComponent *> returnComponentVector;
  for (AddedComponent &c : this->components) {
    returnComponentVector.push_back(c.component);
  }
  return returnComponentVector;
}

void Window::setDimensions(int width, int height) {

  this->width = width;
  this->height = height;
  int distributedWidth = width - (this->xPadding * (this->gridWidth + 1));
  int distrubutedHeight = height - (this->yPadding * (this->gridHeight + 1));
  int optimalCellWidth = (distributedWidth / gridWidth);
  int optimalCellHeight = (distrubutedHeight / gridHeight);
  int widthRemainder = distributedWidth - (optimalCellWidth * this->gridWidth);
  int heightRemainder =
      distrubutedHeight - (optimalCellHeight * this->gridHeight);
  for (int x = 0; x < this->gridWidth; x++) {
    this->cellWidth[x] = optimalCellWidth;
    if (widthRemainder > 0) {
      this->cellWidth[x] += 1;
      widthRemainder--;
    }
  }
  for (int y = 0; y < this->gridHeight; y++) {
    this->cellHeight[y] = optimalCellHeight;
    if (heightRemainder > 0) {
      this->cellHeight[y] += 1;
      heightRemainder--;
    }
  }

  this->repack();
}

int Window::getMaximumHeight(int width) {
  int cellWidth =
      (width - (this->xPadding * (gridWidth + 1))) / this->gridWidth;
  int maxHeight = 0;
  for (AddedComponent &c : this->components) {
    maxHeight =
        maxHeight == 0
            ? (c.component->getMaximumHeight((cellWidth * c.gridWidth) +
                                             this->xPadding *
                                                 (c.gridWidth - 1)) -
               this->yPadding * (c.gridHeight - 1)) /
                  c.gridHeight
            : std::min(maxHeight, c.component->getMaximumHeight(
                                      (cellWidth * c.gridWidth) +
                                      this->xPadding * (c.gridWidth - 1)) /
                                      c.gridHeight);
  }
  if (maxHeight == 0) {
    return INT_MAX;
  }
  return maxHeight * (this->gridHeight) +
         (this->gridHeight + 1) * this->yPadding;
}

int Window::getMaximumWidth() {
  int maxWidth = 0;
  for (AddedComponent &c : this->components) {
    maxWidth = maxWidth == 0
                   ? c.component->getMaximumWidth() / c.gridWidth
                   : std::min(maxWidth, (c.component->getMaximumWidth() -
                                         this->xPadding * (c.gridWidth - 1)) /
                                            c.gridWidth);
  }
  if (maxWidth == 0) {
    return INT_MAX;
  }
  return maxWidth * (this->gridWidth + this->xPadding) + this->xPadding;
}

int Window::getMinimumHeight(int width) {
  int cellWidth =
      (width - (this->xPadding * (gridWidth + 1))) / this->gridWidth;
  int minHeight = 0;
  for (AddedComponent &c : this->components) {
    minHeight = std::max((c.component->getMinimumHeight(
                              cellWidth + this->xPadding * (c.gridWidth - 1)) -
                          this->yPadding * (c.gridHeight - 1)) /
                             c.gridHeight,
                         minHeight);
  }
  return minHeight * (this->gridHeight) +
         (this->gridHeight - 1) * this->yPadding;
}

int Window::getMinimumWidth() {
  int minWidth = 0;
  for (AddedComponent &c : this->components) {
    minWidth = std::max(
        (c.component->getMinimumWidth() - this->xPadding * (c.gridWidth - 1)) /
            (c.gridWidth),
        minWidth);
  }
  return this->gridWidth * (minWidth + this->xPadding) + this->xPadding;
}

void Window::repack() {
  for (AddedComponent &c : this->components) {
    c.width = this->calculateWidth(c);
    c.height = this->calculateHeight(c);
    c.x = this->calculateX(c);
    c.y = this->calculateY(c);
    c.component->setDimensions(c.width, c.height);
  }
}

Window *Window::setDraggable() {
  this->draggable = true;
  return this;
}

bool Window::isDraggable() { return this->draggable; }

int Window::calculateX(AddedComponent &wc) {
  int x = this->xPadding;
  for (int i = 0; i < wc.gridX; i++) {
    x += cellWidth[i] + this->xPadding;
  }
  return x;
}

int Window::calculateY(AddedComponent &wc) {
  int y = this->yPadding;
  for (int i = 0; i < wc.gridY; i++) {
    y += cellHeight[i] + this->yPadding;
  }
  return y;
}

int Window::calculateWidth(AddedComponent &wc) {
  int width = 0;
  for (int i = wc.gridX; i < wc.gridX + wc.gridWidth; i++) {
    width += cellWidth[i] + this->xPadding;
  }
  width -= this->xPadding; // remove padding added after final grid
  return width;
}

int Window::calculateHeight(AddedComponent &wc) {
  int height = 0;
  for (int i = wc.gridY; i < wc.gridY + wc.gridHeight; i++) {
    height += cellHeight[i] + this->yPadding;
  }
  height -= this->yPadding; // remove padding added after final grid
  return height;
}