#include "PaletteWindow.hpp"
#include "../colors.hpp"
#include "../components/SelectorComponent.hpp"
const uint8_t COLORS_PER_ROW = 10;
const uint8_t ROWS = std::ceil((float)colorCount / COLORS_PER_ROW);

PaletteWindow::PaletteWindow(SandComponent *sc, WindowManager *wm)
    : Window(COLORS_PER_ROW, ROWS + 1, 5, 5, true, wm) {
  this->sc = sc;
  for (int i = 0; i < colorCount; i++) {
    this->addComponent(
        new ColorSelectorComponent(colorValues[i],
                                   this->sc->getSandColor() == colorValues[i],
                                   [this](uint32_t color) {
                                     this->sc->setShuffle(false);
                                     this->sc->setSandColor(color);
                                     this->wm->removeWindow(this);
                                   }),
        i % COLORS_PER_ROW, i / COLORS_PER_ROW, 1, 1);
  }
  this->addComponent(new SelectorComponent(
                         "Shuffle",
                         [this](bool selcted) {
                           this->sc->setShuffle(selcted);
                           this->wm->removeWindow(this);
                         },
                         this->sc->getShuffle(), false),
                     0, ROWS, COLORS_PER_ROW, 1);
  this->setTemporary();
}