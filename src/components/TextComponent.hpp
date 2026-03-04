#include "windowComponent.hpp"
#include <string>
#include <vector>
class TextComponent : public windowComponent {
private:
  std::vector<std::string> text;
  int scale;
  std::vector<int> xPadding;
  int yPadding;
  uint32_t color;

public:
  TextComponent(std::vector<std::string> text, uint32_t color);
  void draw(uint32_t *buffer, int x, int y, int screen_width) override;
  void setDimensions(int width, int height) override;
  int getMinimumWidth() override;
  int getMaximumWidth() override;
  int getMinimumHeight(int width) override;
  int getMaximumHeight(int width) override;
};