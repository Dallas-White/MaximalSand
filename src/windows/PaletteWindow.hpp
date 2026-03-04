#include "../components/ColorSelectorComponent.hpp"
#include "../components/SandComponent.hpp"
#include "../components/SelectorComponent.hpp"
#include "Window.hpp"
class PaletteWindow : public Window {
  SandComponent *sc;

public:
  PaletteWindow(SandComponent *sc, WindowManager *wm);
};