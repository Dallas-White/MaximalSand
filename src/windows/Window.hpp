#include "../WindowManager.hpp"
#include "../components/windowComponent.hpp"
#include <SDL2/SDL_keycode.h>
#include <vector>
#ifndef WINDOW
#define WINDOW

class AddedComponent {
public:
  int gridX;
  int gridY;
  int gridWidth;
  int gridHeight;
  int x;
  int y;
  int width;
  int height;
  windowComponent *component;
};
class Window {
private:
  int gridWidth;
  int gridHeight;
  std::vector<int> cellWidth;
  std::vector<int> cellHeight;
  std::vector<int> cellMinWidth;
  std::vector<int> cellMaxWidth;
  int xPadding;
  int yPadding;
  int width;
  int height;

protected:
  int mouseX = 0;
  int mouseY = 0;
  bool drawBackground = true;
  bool temporary = false;
  WindowManager *wm;

private:
  bool drawBorder;
  bool drawMouse = false;
  bool draggable = false;
  std::vector<AddedComponent> components;
  AddedComponent *activeComponent = NULL;
  AddedComponent *mouseComponent = NULL;
  AddedComponent *findMouseComponent(int x, int y);

public:
  Window(int gridWidth, int gridHeight, int xPadding, int yPadding,
         bool drawBorder, WindowManager *wm);
  ~Window();
  Window *setTemporary();
  Window *setDraggable();
  bool isDraggable();
  bool inBound(AddedComponent &wc, int x, int y);
  void setDimensions(int width, int height);
  virtual void handleKeyPress(SDL_Keysym key);
  virtual void draw(uint32_t *buffer, int x, int y, int screen_width);
  virtual bool handleRightMouseButtonDown(int x, int y);
  virtual bool handleRightMouseButtonUp(int x, int y);
  virtual bool handleLeftMouseButtonDown(int x, int y);
  virtual bool handleLeftMouseButtonUp(int x, int y);
  virtual void handleMouseLeave();
  virtual void handleMouseEnter();
  virtual void setMousePosition(int x, int y);
  void addComponent(windowComponent *component, int gridX, int gridY,
                    int gridWidth, int gridHeight);
  void removeComponent(windowComponent *component);
  std::vector<windowComponent *> getComponents();
  int getMinimumHeight(int width);
  int getMaximumHeight(int width);
  int getMinimumWidth();
  int getMaximumWidth();
  void repack();

  int calculateX(AddedComponent &wc);
  int calculateY(AddedComponent &wc);
  int calculateWidth(AddedComponent &wc);
  int calculateHeight(AddedComponent &wc);
};

#endif