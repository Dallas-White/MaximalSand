#include "windowComponent.hpp"
#include <functional>
class ColorSelectorComponent : public windowComponent {
private:
  uint32_t color;
  bool selected;
  bool hovering;
  std::function<void(uint32_t)> callback;

public:
  ColorSelectorComponent(uint32_t color, bool selected,
                         std::function<void(uint32_t color)> onSelect);
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  bool handleLeftMouseButtonDown(int x, int y) override;
  void handleMouseEnter() override;
  void handleMouseLeave() override;
  bool handleLeftMouseButtonUp(int x, int y) override;
  bool isSelected();
  void setSelected(bool selcted);
  uint32_t getColor();
  int getMinimumWidth() override;
  int getMinimumHeight(int width) override;
  int getMaximumHeight(int width) override;
  int getMaximumWidth() override;
  void setDimensions(int width, int height) override;
};