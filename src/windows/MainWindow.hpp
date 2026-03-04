#include "../components/SandComponent.hpp"
#include "Window.hpp"
class MainWindow : public Window {
private:
  SandComponent *sandComponent;

public:
  MainWindow(WindowManager *wm);
};