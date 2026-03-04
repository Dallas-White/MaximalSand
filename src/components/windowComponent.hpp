#include <SDL2/SDL_events.h>

#ifndef WINDOWCOMPONENT
#define WINDOWCOMPONENT
class windowComponent {
protected:
  int x;
  int y;
  int width;
  int height;
  int mouseX;
  int mouseY;

public:
  virtual ~windowComponent() = default;
  virtual void handleKeyPress(SDL_Keysym code) {};
  virtual void draw(uint32_t *buffer, int x, int y, int screen_width) = 0;
  virtual bool handleLeftMouseButtonDown(int x, int y) { return false; };
  virtual bool handleRightMouseButtonDown(int x, int y) { return false; };
  virtual bool handleLeftMouseButtonUp(int x, int y) { return false; };
  virtual bool handleRightMouseButtonUp(int x, int y) { return false; };
  virtual void handleMouseLeave() {};
  virtual void handleMouseEnter() {};
  virtual bool drawsMouse() { return false; };
  virtual void setMousePosition(int x, int y) {
    this->mouseX = x;
    this->mouseY = y;
  };
  virtual void setDimensions(int width, int height) = 0;
  virtual void setPosition(int x, int y) {
    this->x = x;
    this->y = y;
  };
  virtual int getX() { return this->x; };
  virtual int getY() { return this->y; };
  virtual int getWidth() { return this->width; };
  virtual int getHeight() { return this->height; };
  virtual int getMaximumWidth() = 0;
  virtual int getMaximumHeight(int width) = 0;
  virtual int getMinimumWidth() = 0;
  virtual int getMinimumHeight(int width) = 0;
};

#endif