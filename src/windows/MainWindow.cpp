#include "MainWindow.hpp"
#include "../components/ButtonComponent.hpp"
#include "../components/ColorComponent.hpp"
#include "../components/ColorShowComponent.hpp"
#include "../components/SelectorComponent.hpp"
#include "../utils.hpp"
#include "ClearConfirmationWindow.hpp"
#include "HelpWindow.hpp"
MainWindow::MainWindow(WindowManager *wm) : Window(15, 20, 0, 0, false, wm) {
  this->sandComponent = new SandComponent();
  this->addComponent(new ColorComponent(rgb_to_uint32(10, 10, 10)), 0, 0, 15,
                     1);
  this->addComponent(sandComponent, 0, 1, 15, 19);
  this->addComponent(new ColorShowComponent(this->sandComponent, wm, 1), 0, 0,
                     1, 1);
  this->addComponent(
      new ButtonComponent("+", true, 255, 255, 255,
                          [this] {
                            this->sandComponent->setBrushSize(std::min(
                                255, this->sandComponent->getBrushSize() + 10));
                          }),
      1, 0, 1, 1);
  this->addComponent(
      new ButtonComponent("-", true, 255, 255, 255,
                          [this] {
                            this->sandComponent->setBrushSize(std::max(
                                1, this->sandComponent->getBrushSize() - 10));
                          }),
      2, 0, 1, 1);
  this->addComponent(new SelectorComponent(
                         "Erase",
                         [this](bool selected) {
                           this->sandComponent->setEraseMode(selected);
                         },
                         false),
                     3, 0, 3, 1);
  this->addComponent(
      new ButtonComponent("Flip", true, 255, 255, 255,
                          [this] { this->sandComponent->InvertGravity(); }),
      6, 0, 3, 1);
  this->addComponent(
      new ButtonComponent(
          "Clear", true, 255, 255, 255,
          [this]() {
            int clearWindowWidth = std::max(320, this->wm->getWidth() / 2);
            int clearWindowHeight = std::max(100, this->wm->getWidth() / 8);
            int cwX, cwY;
            calculateCenteredItem(0, 0, this->wm->getWidth(),
                                  this->wm->getHeight(), clearWindowWidth,
                                  clearWindowHeight, cwX, cwY);
            this->wm->addForegroundWindow(
                new ClearConfirmationWindow(this->wm, this->sandComponent), cwX,
                cwY, clearWindowWidth, clearWindowHeight);
          }),
      9, 0, 3, 1);

  this->addComponent(
      new ButtonComponent(
          "Help", true, 255, 255, 255,
          [this] {
            Window *helpW = new HelpWindow(this->wm);
            int helpWindowWidth = std::max(320, this->wm->getWidth() / 4);
            int helpWindowHeight =
                std::max(helpW->getMinimumHeight(helpWindowWidth),
                         this->wm->getHeight() / 2);
            int hwX, hwY;
            calculateCenteredItem(0, 0, this->wm->getWidth(),
                                  this->wm->getHeight(), helpWindowWidth,
                                  helpWindowHeight, hwX, hwY);
            this->wm->addForegroundWindow(helpW, hwX, hwY, helpWindowWidth,
                                          helpWindowHeight);
          }),
      12, 0, 3, 1);
  this->drawBackground = false;
}