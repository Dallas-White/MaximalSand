#include "FallbackWindow.hpp"
#include "../components/ButtonComponent.hpp"
#include "../components/TextComponent.hpp"
#include "../components/windowComponent.hpp"
#include "../utils.hpp"
FallbackWindow::FallbackWindow(WindowManager *wm)
    : Window(3, 3, 5, 5, true, wm) {
  this->addComponent(new TextComponent({"Your device/browser does not",
                                        "support multithreading",
                                        "performance will be degraded"},
                                       rgb_to_uint32(255, 255, 0)),
                     0, 0, 3, 2);
  this->addComponent(
      new ButtonComponent("OK", true, 255, 255, 255,
                          [this] { this->wm->removeWindow(this); }),
      1, 2, 1, 1);
}