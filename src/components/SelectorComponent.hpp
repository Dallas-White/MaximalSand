#include "../utils.hpp"
#include "windowComponent.hpp"
#include <SDL2/SDL_keycode.h>
#include <cstdint>
#include <functional>
#include <string>
#ifndef SELECTOR_COMPONENT
#define SELECTOR_COMPONENT
class SelectorComponent : public windowComponent {
private:
  bool selected;
  bool hovering;
  int xPadding;
  int yPadding;
  int textScale;
  bool allowUnselecting;
  uint32_t selectedColor;
  std::string text;
  std::function<void(bool)> callback;

public:
  SelectorComponent(std::string text, std::function<void(bool)> callback,
                    bool selected, bool allowUnselecting = true,
                    uint32_t selectedColor = rgb_to_uint32(128, 255, 128));
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  void handleMouseEnter() override;
  void handleMouseLeave() override;
  bool handleLeftMouseButtonDown(int x, int y) override;
  bool handleLeftMouseButtonUp(int x, int y) override;
  void setSelected(bool selected);
  bool isSelected();
  int getMinimumHeight(int width) override;
  int getMinimumWidth() override;
  int getMaximumHeight(int width) override;
  int getMaximumWidth() override;
  void setDimensions(int width, int height) override;
};

#endif