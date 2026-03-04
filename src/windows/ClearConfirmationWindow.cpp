#include "ClearConfirmationWindow.hpp"
#include "../components/ButtonComponent.hpp"
#include "../components/TextComponent.hpp"
ClearConfirmationWindow::ClearConfirmationWindow(WindowManager *wm,
                                                 SandComponent *sc)
    : Window(2, 3, 5, 5, true, wm) {
  this->addComponent(
      new TextComponent({"Are You Sure You ", "Want To Clear The Screen"},
                        rgb_to_uint32(255, 255, 255)),
      0, 0, 2, 1);
  this->addComponent(
      new ButtonComponent("No", true, 255, 255, 255,
                          [this] { this->wm->removeWindow(this); }),
      0, 2, 1, 1);
  this->addComponent(new ButtonComponent("Yes", true, 255, 255, 255,
                                         [this, sc] {
                                           sc->clear();
                                           this->wm->removeWindow(this);
                                         }),
                     1, 2, 1, 1);
}