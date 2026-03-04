#include "WindowManager.hpp"
#include "components/TextComponent.hpp"
#include "windows/FallbackWindow.hpp"
#include "windows/MainWindow.hpp"
#include "windows/Window.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_touch.h>
#include <SDL2/SDL_video.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

#endif

const int MINIMUM_WIDTH = 320;

const int MINIMUM_HEIGHT = 480;

int newSandColor;
int width = 1600;
int height = 800;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
WindowManager *wm;
Uint32 *outputPixels = new Uint32[width * height];
bool running = true;
uint32_t mouseState;

#ifdef __EMSCRIPTEN__
static EM_BOOL on_resize(int eventType, const EmscriptenUiEvent *e, void *) {
  int w, h;
  emscripten_get_canvas_element_size("#canvas", &w, &h);
  SDL_SetWindowSize(window, w, h);
  return EM_TRUE;
}

#endif

void handleResize() {
  SDL_SetWindowSize(window, width, height);
  SDL_DestroyTexture(texture);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  delete[] outputPixels;
  outputPixels = new Uint32[width * height];
  memset(outputPixels, 0, width * height * sizeof(uint32_t));

  SDL_RenderSetLogicalSize(renderer, width, height);
  wm->setDimensions(width, height);
}
inline void loop() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      running = false;
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
      if (event.button.button == SDL_BUTTON_LEFT) {
        wm->handleLeftMouseButtonDown(event.motion.x, event.motion.y);
      } else if (event.button.button == SDL_BUTTON_RIGHT) {
        wm->handleRightMouseButtonDown(event.motion.x, event.motion.y);
      }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
      if (event.button.button == SDL_BUTTON_LEFT) {
        wm->handleLeftMouseButtonUp(event.motion.x, event.motion.y);
      } else if (event.button.button == SDL_BUTTON_RIGHT) {
        wm->handleRightMouseButtonUp(event.motion.x, event.motion.y);
      }
    } else if (event.type == SDL_WINDOWEVENT &&
               event.window.event == SDL_WINDOWEVENT_RESIZED) {
      width = std::max(event.window.data1, MINIMUM_WIDTH);
      height = std::max(event.window.data2, MINIMUM_HEIGHT);
      handleResize();
    } else if (event.type == SDL_KEYDOWN) {
      wm->handleKeyPress(event.key.keysym);
    } else if (event.type == SDL_MOUSEMOTION) {
      int mouseX = event.motion.x;
      int mouseY = event.motion.y;
      if (mouseX == 0 || mouseY == 0 || mouseX == width - 1 ||
          mouseY == height - 1) {

        wm->setMousePosition(-1, -1);
      } else {

        wm->setMousePosition(mouseX, mouseY);
      }
    }
  }
  int mouseX;
  int mouseY;

  wm->draw(outputPixels);
  SDL_UpdateTexture(texture, nullptr, outputPixels, width * sizeof(Uint32));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }
  window = SDL_CreateWindow("Maximal Sand", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  SDL_ShowCursor(SDL_DISABLE);
  wm = new WindowManager();
  wm->setDimensions(width, height);
  wm->setBackgroundWindow(new MainWindow(wm));

#ifdef SINGLETHREAD
  wm->addWindow(new FallbackWindow(wm), (width / 2) - (width / 4),
                (height / 2 - height / 8), width / 2, height / 4);
#endif

#ifdef __EMSCRIPTEN__

  SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr,
                                 EM_TRUE, on_resize);

  emscripten_set_main_loop(loop, 0, 1);
#endif
  SDL_SetWindowMinimumSize(window, MINIMUM_WIDTH, MINIMUM_HEIGHT);
#ifndef __EMSCRIPTEN__
  while (running) {
    loop();
  }
#endif

  delete[] outputPixels;
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
