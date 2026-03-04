#include "windowComponent.hpp"
#include <SDL2/SDL_keycode.h>
#include <functional>
#include <string>
class ButtonComponent : public windowComponent {
private:
  bool hovering;
  uint32_t foregroundColor;
  uint32_t dimmedColor;
  std::function<void()> callback;
  bool showBorder;
  std::string text;
  int textScale;
  int horizontalPadding;
  int verticalPadding;
  bool hasKeybind = false;

public:
  ButtonComponent(std::string text, bool showBorder, uint8_t foregroundR,
                  uint8_t foregroundG, uint8_t foregroundB,
                  std::function<void()> callback);
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  bool handleLeftMouseButtonUp(int x, int y) override;
  bool handleLeftMouseButtonDown(int x, int y) override;
  void handleMouseLeave() override;
  void handleMouseEnter() override;
  int getMinimumWidth() override;
  int getMinimumHeight(int width) override;
  int getMaximumHeight(int width) override;
  int getMaximumWidth() override;
  void setDimensions(int w, int h) override;
};
