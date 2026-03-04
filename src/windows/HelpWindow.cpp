#include "HelpWindow.hpp"
#include "../components/ButtonComponent.hpp"
#include "../components/TextComponent.hpp"
#include "../utils.hpp"
HelpWindow::HelpWindow(WindowManager *wm) : Window(2, 9, 5, 5, true, wm) {
  this->addComponent(new TextComponent({"Maximal Sand", "By: Dallas White"},
                                       rgb_to_uint32(0, 0, 255)),
                     0, 0, 2, 3);
  this->addComponent(
      new TextComponent({"Keybindings"}, rgb_to_uint32(255, 255, 255)), 0, 4, 2,
      1);

  this->addComponent(
      new TextComponent({"+: Increase brush size", "-: Decrease brush size",
                         "space: Flip sand"},
                        rgb_to_uint32(255, 255, 255)),
      0, 5, 2, 2);
  this->addComponent(
      new ButtonComponent("Close", true, 255, 255, 255,
                          [this] { this->wm->removeWindow(this); }),
      0, 8, 2, 1);
}
