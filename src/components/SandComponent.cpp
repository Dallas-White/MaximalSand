#include "SandComponent.hpp"
#include "../colors.hpp"
#include "../sand.hpp"
#include "../utils.hpp"
#include "../windows/Window.hpp"
#include <SDL2/SDL_keycode.h>

Uint32 drift_pixel(Uint32 color) {
  Uint8 r;
  Uint8 g;
  Uint8 b;
  uint32_to_rgb(color, &r, &g, &b);

  r = fmax(fmin(r + (random() % 50 + 25), 255), 0);
  b = fmax(fmin(b + (random() % 50 + 25), 255), 0);
  g = fmax(fmin(g + (random() % 50 + 25), 255), 0);
  Uint32 result = rgb_to_uint32(r, g, b);

  return result;
}

SandComponent::SandComponent()
    : sandSim(10, 10),
      windowComponent() { // The sand sim is initalized to 10
                          // here as a filler value, the Window will resize the
                          // component and the simulator
  this->sandColor = colorValues[this->rng.next() % colorCount];
};

void SandComponent::handleMouseLeave() {
  this->drawMouse = false;
  this->placing = false;
  this->deleting = false;
}

void SandComponent::handleMouseEnter() {
  this->drawMouse = true;
  this->showBrushSize = false;
}

bool SandComponent::handleRightMouseButtonDown(int x, int y) {
  this->deleting = true;
  return true;
}

bool SandComponent::handleRightMouseButtonUp(int x, int y) {
  this->deleting = false;
  this->placing = false;
  if (this->shuffle)
    this->pickNewColor();
  return true;
}

bool SandComponent::handleLeftMouseButtonDown(int x, int y) {
  if (this->eraseMode)
    this->deleting = true;
  else
    this->placing = true;
  return true;
}

bool SandComponent::handleLeftMouseButtonUp(int x, int y) {
  this->placing = false;
  this->deleting = false;
  if (this->shuffle)
    this->pickNewColor();
  return true;
}

void SandComponent::handleKeyPress(SDL_Keysym code) {
  if (code.sym == SDLK_SPACE) {
    sandSim.invertGravity();
  } else if (code.sym == SDLK_PLUS || code.sym == SDLK_KP_PLUS ||
             ((code.sym == SDLK_EQUALS || code.sym == SDLK_KP_PLUSMINUS) &&
              code.mod & KMOD_SHIFT)) {
    brushSize = std::min(brushSize + 10, 255);
  } else if (code.sym == SDLK_MINUS || code.sym == SDLK_KP_MINUS ||
             code.sym == SDLK_KP_PLUSMINUS) {
    brushSize = std::max(brushSize - 10, 1);
  }
}
void SandComponent::setDimensions(int width, int height) {
  this->width = width;
  this->height = height;
  this->sandSim.setDimensions(this->width, this->height);
#ifdef SINGLETHREAD
  sandSim.run(); // Run the sandsim for 1 frame to set the dimensions if we are
                 // running in single threaded mode
#endif
}

void SandComponent::draw(uint32_t *buffer, int x, int y, int screen_width) {
  rng.next();
  if (this->deleting) {
    if (mouseX - x >= 0 && mouseX - x < this->getWidth() && mouseY - y >= 0 &&
        mouseY - y < this->getHeight()) {
      for (int xOff = -brushSize / 2; xOff <= brushSize / 2; xOff++) {
        for (int yOff = -brushSize / 2; yOff <= brushSize / 2; yOff++) {
          if (!sandSim.inBound(mouseX + xOff - x, mouseY + yOff - y))
            continue;
          sandSim.deleteSand(mouseX + xOff - x, mouseY + yOff - y);
        }
      }
    }
  }
  if (this->placing) {
    if (mouseX - x >= 0 && mouseX - x < this->getWidth() && mouseY - y >= 0 &&
        mouseY - y < this->getHeight()) {
      for (int xOff = -brushSize / 2; xOff <= brushSize / 2; xOff++) {
        for (int yOff = -brushSize / 2; yOff <= brushSize / 2; yOff++) {
          if (!sandSim.inBound(mouseX + xOff - x, mouseY + yOff - y) ||
              sandSim.getPixel(mouseX + xOff - x, mouseY + yOff - y) != 0 ||
              random() % 5 != 1)
            continue;
          sandSim.createSand(mouseX + xOff - x, mouseY + yOff - y,
                             drift_pixel(this->sandColor));
        }
      }
    }
  }
  sandSim.draw(buffer, x, y, width, height, screen_width);

  if (this->drawMouse) {
    for (int xOff = -brushSize / 2 - x; xOff <= brushSize / 2 + x; xOff++) {
      int cx = this->mouseX + xOff + x;
      if (cx >= x && cx < x + this->getWidth()) {
        int yTop = this->mouseY + brushSize / 2;
        if (yTop >= y && yTop < y + this->getHeight())
          buffer[yTop * screen_width + cx] = this->sandColor;

        int yBot = this->mouseY - brushSize / 2;
        if (yBot >= y && yBot < y + this->getHeight())
          buffer[yBot * screen_width + cx] = this->sandColor;
      }
    }

    for (int yOff = -brushSize / 2; yOff <= brushSize / 2; yOff++) {
      int cy = this->mouseY + yOff;
      if (cy >= y && cy < y + this->getHeight()) {
        int xRight = this->mouseX + brushSize / 2 + x;
        if (xRight >= x && xRight < x + this->getWidth())
          buffer[cy * screen_width + xRight] = this->sandColor;

        int xLeft = this->mouseX - brushSize / 2 + x;
        if (xLeft >= x && xLeft < x + this->getWidth())
          buffer[cy * screen_width + xLeft] = this->sandColor;
      }
    }
  }
  if (this->showBrushSize) {
    int brushX, brushY;
    calculateCenteredItem(x, y, width, height, brushSize, brushSize, brushX,
                          brushY);
    for (int bx = brushX; bx <= brushX + brushSize; bx++) {
      for (int by = brushY; by <= brushY + brushSize; by++) {
        buffer[by * screen_width + bx] = this->sandColor;
      }
    }
  }

#ifdef SINGLETHREAD
  sandSim.run();
#endif
}

void SandComponent::setSandColor(uint32_t new_color) {

  this->shuffle = false;
  this->sandColor = new_color;
}

uint32_t SandComponent::getSandColor() { return this->sandColor; }

bool SandComponent::getShuffle() { return this->shuffle; }

void SandComponent::setShuffle(bool shuffle) { this->shuffle = shuffle; }

bool SandComponent::drawsMouse() { return true; }

void SandComponent::InvertGravity() { this->sandSim.invertGravity(); }

uint8_t SandComponent::getBrushSize() { return this->brushSize; }

void SandComponent::setBrushSize(uint8_t brushSize) {
  this->showBrushSize = true;
  this->brushSize = brushSize;
}

void SandComponent::pickNewColor() {
  uint32_t new_color;
  do {
    new_color = colorValues[this->rng.next() % colorCount];
  } while (new_color == this->sandColor);
  this->sandColor = new_color;
}

int SandComponent::getWidth() { return this->width; }

int SandComponent::getHeight() { return this->height; }

int SandComponent::getMaximumHeight(int width) { return INT_MAX; }

int SandComponent::getMaximumWidth() { return INT_MAX; }

int SandComponent::getMinimumWidth() { return 1; }

int SandComponent::getMinimumHeight(int width) { return 1; }

void SandComponent::setEraseMode(bool eraseMode) {
  this->eraseMode = eraseMode;
}

void SandComponent::clear() { this->sandSim.clearBoard(); }