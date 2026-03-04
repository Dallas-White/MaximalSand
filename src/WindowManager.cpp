#include "WindowManager.hpp"
#include "windows/Window.hpp"
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <cstring>

bool inBound(AddedWindow &wc, int x, int y) {
  return x >= wc.x && x < wc.x + wc.width && y >= wc.y && y < wc.y + wc.height;
}
void WindowManager::setDimensions(int width, int height) {
  this->width = width;
  this->height = height;
  if (this->windows.size() < 1) {
    return;
  }
  AddedWindow *backgroundWindow = &this->windows[this->windows.size() - 1];
  backgroundWindow->window->setDimensions(width, height);
  backgroundWindow->width = width;
  backgroundWindow->height = height;
  backgroundWindow->x = 0;
  backgroundWindow->y = 0;
  for (int x = 0; x < this->windows.size() - 1; x++) {
    AddedWindow &w = this->windows[x];
    if (w.width > this->width || w.height > this->height) {
      int oldHeight = w.height;
      w.width = std::min(w.width, this->width);
      w.height = std::min(w.height, this->height);
      w.height = std::min(w.window->getMaximumHeight(w.width), w.height);
      while (w.height < w.window->getMinimumHeight(w.width)) {
        w.width--;
        if (w.width <= 0) {
          w.width = this->width;
          oldHeight--;
          w.height = oldHeight;
        }
      }
      w.window->setDimensions(w.width, w.height);
    }
    w.x = std::max(std::min(w.x, this->width - w.width), 0);
    w.y = std::max(std::min(w.y, this->height - w.height), 0);
  }
}

void WindowManager::handleKeyPress(SDL_Keysym key) {
  for (AddedWindow &w : this->windows) {
    w.window->handleKeyPress(key);
  }
}

AddedWindow *WindowManager::findMouseWindow(int x, int y) {
  for (AddedWindow &w : this->windows) {
    if (inBound(w, x, y)) {
      return &w;
    }
  }
  return NULL;
}

void WindowManager::draw(uint32_t *buffer) {
  memset(buffer, 0, this->width * this->height * sizeof(uint32_t));
  for (int x = this->windows.size() - 1; x >= 0; x--) {
    AddedWindow &w = this->windows[x];
    w.window->draw(buffer, w.x, w.y, this->width);
  }
}

void WindowManager::handleLeftMouseButtonDown(int x, int y) {
  if (this->mouseWindow) {
    this->moveToFront(mouseWindow->window);
    if (this->mouseWindow->window->handleLeftMouseButtonDown(
            x - this->mouseWindow->x, y - this->mouseWindow->y)) {
      return;
    } else if (this->mouseWindow->window->isDraggable()) {
      this->mouseWindow->dragging = true;
    }
  }
}

void WindowManager::handleRightMouseButtonDown(int x, int y) {
  if (mouseWindow != NULL) {
    this->moveToFront(mouseWindow->window);
    if (this->mouseWindow->window->handleRightMouseButtonDown(
            x - this->mouseWindow->x, y - this->mouseWindow->y)) {
      return;
    }
  }
}

void WindowManager::handleLeftMouseButtonUp(int x, int y) {
  if (mouseWindow != NULL && mouseWindow->dragging) {
    mouseWindow->dragging = false;
  } else if (mouseWindow != NULL) {
    mouseWindow->window->handleLeftMouseButtonUp(x - mouseWindow->x,
                                                 y - mouseWindow->y);
  }
}

void WindowManager::handleRightMouseButtonUp(int x, int y) {
  if (mouseWindow != NULL) {
    mouseWindow->window->handleRightMouseButtonUp(x, y);
  }
}

void WindowManager::setMousePosition(int x, int y) {

  if (this->mouseWindow != NULL && mouseWindow->dragging) {
    int dx = x - this->mouseX;
    int dy = y - this->mouseY;
    mouseWindow->x =
        std::clamp(dx + mouseWindow->x, 0, this->width - mouseWindow->width);
    mouseWindow->y =
        std::clamp(dy + mouseWindow->y, 0, this->height - mouseWindow->height);
  }
  AddedWindow *newMouseWindow = findMouseWindow(x, y);
  if (newMouseWindow != this->mouseWindow) {
    if (this->mouseWindow != NULL) {
      this->mouseWindow->dragging = false;
      this->mouseWindow->window->handleMouseLeave();
    }
    if (newMouseWindow != NULL)
      newMouseWindow->window->handleMouseEnter();
    this->mouseWindow = newMouseWindow;
  }

  if (this->mouseWindow != NULL)
    mouseWindow->window->setMousePosition(x - mouseWindow->x,
                                          y - mouseWindow->y);
  this->mouseX = x;
  this->mouseY = y;
}
int WindowManager::getMinimumHeight() { return 100; }

int WindowManager::getMinimumWidth() { return 100; }

void WindowManager::addWindow(Window *w, int x, int y, int width, int height) {
  AddedWindow aw;
  aw.window = w;
  aw.x = x;
  aw.y = y;
  aw.width = width;
  aw.height = height;
  w->setDimensions(width, height);
  this->windows.insert(this->windows.begin() + foregroundWindowCount, aw);
  this->mouseWindow = this->findMouseWindow(this->mouseX, this->mouseY);
}

void WindowManager::addForegroundWindow(Window *w, int x, int y, int width,
                                        int height) {
  AddedWindow aw;
  aw.window = w;
  aw.x = x;
  aw.y = y;
  aw.width = width;
  aw.height = height;
  w->setDimensions(width, height);
  this->windows.insert(this->windows.begin(), aw);
  this->foregroundWindowCount++;
  this->mouseWindow = this->findMouseWindow(this->mouseX, this->mouseY);
}

Window *WindowManager::setBackgroundWindow(Window *w) {
  AddedWindow aw;
  aw.window = w;
  aw.x = 0;
  aw.y = 0;
  aw.width = this->width;
  aw.height = this->height;
  aw.draggable = false;
  w->setDimensions(width, height);
  this->windows.push_back(aw);
  this->mouseWindow = this->findMouseWindow(this->mouseX, this->mouseY);
  return NULL;
}

void WindowManager::moveToBack(Window *w) {
  if (w == this->windows[this->windows.size() - 1].window)
    return; // The background window should not be moved
  for (int x = 0; x < this->windows.size(); x++) {
    if (this->windows[x].window == w) {
      AddedWindow newW = this->windows[x];
      this->windows.erase(this->windows.begin() + x);
      if (x < foregroundWindowCount) {
        this->windows.insert(this->windows.begin() + foregroundWindowCount - 1,
                             newW);
      } else {
        this->windows.insert(this->windows.end() - 1, newW);
      }
      break;
    }
  }
}

void WindowManager::moveToFront(Window *w) {
  if (w == this->windows[this->windows.size() - 1].window)
    return; // The background window should not be moved
  for (int x = 0; x < this->windows.size(); x++) {
    if (this->windows[x].window == w) {
      AddedWindow newW = this->windows[x];
      this->windows.erase(this->windows.begin() + x);
      if (x < foregroundWindowCount) {
        this->windows.insert(this->windows.begin(), newW);
      } else {
        this->windows.insert(this->windows.begin() + foregroundWindowCount,
                             newW);
      }
      break;
    }
  }
}

void WindowManager::removeWindow(Window *w) {
  for (int x = 0; x < this->windows.size(); x++) {
    if (this->windows[x].window == w) {
      this->windows.erase(this->windows.begin() + x);
      if (x < foregroundWindowCount) {
        foregroundWindowCount--;
      }
      if (&this->windows[x] == mouseWindow) {
        mouseWindow = NULL;
      }
      break;
    }
  }
  this->setMousePosition(this->mouseX,
                         this->mouseY); // this handles finding the active
                                        // widnow and mouse Enter/Leave events
}

int WindowManager::getWidth() { return this->width; }

int WindowManager::getHeight() { return this->height; }

void WindowManager::setWindowPosition(Window *w, int x, int y) {
  for (AddedWindow &aw : this->windows) {
    if (aw.window == w) {
      aw.x = x;
      aw.y = y;
    }
  }
}