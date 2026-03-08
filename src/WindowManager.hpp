// Forward declare Window class

#include <SDL2/SDL_keyboard.h>
class Window;
#include <SDL2/SDL_keycode.h>
#include <vector>

#ifndef WINDOWMANAGER
#define WINDOWMANAGER
class AddedWindow {
public:
  Window *window;
  int x;
  int y;
  int width;
  int height;
  bool dragging = false;
  bool draggable = false;
};

class WindowManager {
private:
  int width;
  int mouseX;
  int mouseY;
  int height;
  int foregroundWindowCount = 0;
  std::vector<AddedWindow> windows;
  AddedWindow *mouseWindow = NULL;
  AddedWindow *findMouseWindow(int x, int y);

public:
  void setDimensions(int width, int height);
  void handleKeyPress(SDL_Keysym key);
  void draw(uint32_t *buffer);
  void handleRightMouseButtonDown(int x, int y);
  void handleRightMouseButtonUp(int x, int y);
  void handleLeftMouseButtonDown(int x, int y);
  void handleLeftMouseButtonUp(int x, int y);
  void setMousePosition(int x, int y);
  int getMinimumHeight();
  int getMinimumWidth();
  int getWidth();
  int getHeight();
  void addWindow(Window *, int x, int y, int w, int h);
  void addForegroundWindow(Window *, int x, int y, int w, int h);
  Window *setBackgroundWindow(Window *);
  void moveToFront(Window *);
  void moveToBack(Window *);
  void removeWindow(Window *);
  void setWindowPosition(Window *, int x, int y);
  void setWindowDimensions(Window *, int x, int y);
  template <typename W> bool isWindowOpen() {
    for (AddedWindow &aw : this->windows) {
      if (dynamic_cast<W *>(aw.window)) {
        return true;
      }
    }
    return false;
  }
};
#endif