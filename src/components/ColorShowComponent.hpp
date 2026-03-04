#include "../WindowManager.hpp"
#include "SandComponent.hpp"
#include "windowComponent.hpp"
class ColorShowComponent : public windowComponent {
private:
  SandComponent *sw;
  int padding;
  bool hovering = false;
  int width;
  int height;
  WindowManager *wm;

public:
  ColorShowComponent(SandComponent *sw, WindowManager *wm, int padding);
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  void handleMouseEnter() override;
  void handleMouseLeave() override;
  bool handleLeftMouseButtonDown(int x, int y) override;
  bool handleLeftMouseButtonUp(int x, int y) override;
  int getMaximumHeight(int width) override;
  int getMaximumWidth() override;
  int getMinimumHeight(int width) override;
  int getMinimumWidth() override;
  void setDimensions(int width, int height) override;
};