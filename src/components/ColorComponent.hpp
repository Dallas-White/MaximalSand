#include "windowComponent.hpp"
class ColorComponent : public windowComponent {
private:
  Uint32 color; // The color of the component
  int width;
  int height;
  bool hovering = false;

public:
  ColorComponent(Uint32 color);
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  int getMaximumHeight(int) override;
  int getMaximumWidth() override;
  int getMinimumHeight(int width) override;
  int getMinimumWidth() override;
  void setDimensions(int width, int height) override;
};