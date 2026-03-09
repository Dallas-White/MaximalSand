#include "sand.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <barrier>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <thread>

const bool COUNT_SAND_DEBUG = false;
const bool BOUNCE_SAND_DEBUG = false;
const bool SHOW_ACTIVE_CHUNKS_DEBUG = false;
const int THREAD_COUNT = 4;
const int FRAMERATE_LIMIT = 480;

void Sand::onFinishFrame() {
  this->frameLock.lock();
  if (COUNT_SAND_DEBUG) {
    int sandcount = 0;
    for (int i = 0; i < this->width * this->height; i++) {
      if (this->pixels[i] != 0) {
        sandcount++;
      }
    }
    std::cout << "Counted " << sandcount << " grains of sand" << std::endl;
  }
  if (BOUNCE_SAND_DEBUG) {
    bool needsInversion = true;
    Chunk *chunkPointer = this->cm.getChunks();
    for (int x = 0; x < this->cm.getNumChunks(); x++) {
      if (chunkPointer[x].dirty) {
        needsInversion = false;
        break;
      }
    }
    if (needsInversion) {
      this->isGravityInverted = !this->isGravityInverted;
      this->cm.forceBoardRecheck();
    }
  }
  if (resizeRequested) {
    Uint32 *newPixels = new Uint32[pendingW * pendingH];
    memset(newPixels, 0, pendingW * pendingH * sizeof(Uint32));
    Uint32 *newNextFrame = new Uint32[pendingW * pendingH];
    memset(newNextFrame, 0, pendingW * pendingH * sizeof(Uint32));
    this->cm.setGridSize(pendingW, pendingH);
    this->cm.forceBoardRecheck();
    for (int x = 0; x < std::min(this->width, pendingW); x++) {
      for (int y = 0; y < std::min(this->height, pendingH); y++) {
        newPixels[y * pendingW + x] = pixels[y * this->width + x];
      }
    }

    memset(newNextFrame, 0, pendingW * pendingH * sizeof(Uint32));

    delete[] this->pixels;
    delete[] this->nextFrame;
    this->pixels = newPixels;
    this->nextFrame = newNextFrame;
    this->width = pendingW;
    this->height = pendingH;
    this->resizeRequested = false;
  } else if (clearRequested) {

    memset(this->nextFrame, 0, this->width * this->height * sizeof(uint32_t));
    memset(this->pixels, 0, this->width * this->height * sizeof(uint32_t));
    clearRequested = false;
  } else {

    Uint32 *temp = this->pixels;
    this->pixels = this->nextFrame;
    this->nextFrame = temp;
    this->invertX = rand() % 2;
    memset(this->nextFrame, 0, this->width * this->height * sizeof(uint32_t));
  }
  this->cm.startCycle();
  this->frameLock.unlock();
#ifndef SINGLETHREAD
  if (std::chrono::high_resolution_clock().now() - this->frameStartTime <
      std::chrono::milliseconds(1000 / FRAMERATE_LIMIT)) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 / FRAMERATE_LIMIT) -
        (std::chrono::high_resolution_clock().now() - this->frameStartTime));
  }
  this->frameStartTime = std::chrono::high_resolution_clock().now();
#endif
}

#ifndef SINGLETHREAD
void Sand::onBarrierFinished() {

  if (cm.hasNextPhase()) {
    cm.startNextPhase();
  } else {
    this->onFinishFrame();
  }
}
#endif

Sand::Sand(int w, int h) : cm(w, h) {
  this->width = w;
  this->height = h;
  this->pixels = new Uint32[w * h];
  this->nextFrame = new Uint32[w * h];
  memset(pixels, 0, width * height * sizeof(Uint32));
  memset(nextFrame, 0, width * height * sizeof(Uint32));
#ifndef SINGLETHREAD
  this->frameStartTime = std::chrono::high_resolution_clock().now();
  this->barrier = new std::barrier<std::function<void()>>(
      THREAD_COUNT, [this]() { this->onBarrierFinished(); });
  for (int i = 0; i < THREAD_COUNT; i++) {
    this->threads.push_back(std::thread(&Sand::runThread, this));
  }
#endif
  this->cm.startCycle();
  this->clearRequested = false;
  this->isGravityInverted = false;
}

Sand::~Sand() {
  this->width = 0;
#ifndef SINGLETHREAD
  for (int x = 0; x < this->threads.size(); x++) {
    this->threads[x].join();
  }
#endif
  delete[] this->pixels;
}

bool Sand::inBound(int x, int y) {
  return x >= 0 && x < this->width && y >= 0 && y < this->height;
}

bool Sand::inBound(int x, int y, int w, int h) {

  return x >= 0 && x < w && y >= 0 && y < h;
}

void Sand::draw(Uint32 *buffer, int x1, int y1, int width, int height,
                int screen_width) {
  this->frameLock.lock();
  int copyRowSize = std::min(this->width, width) * sizeof(Uint32);
  for (int y = 0; y < std::min(this->height, height); y++) {
    Uint32 *srcRow = &this->pixels[y * this->width];

    Uint32 *dstRow = &buffer[(y + y1) * screen_width + x1];

    std::memcpy(dstRow, srcRow, copyRowSize);

    this->frameLock.unlock();
  }
}

void Sand::createSand(int x, int y, Uint32 color) {
  this->frameLock.lock();
  this->pixels[y * this->width + x] = color;
  this->nextFrame[y * this->width + x] = color;
  this->cm.setDirty(x, y);
  this->frameLock.unlock();
}

bool Sand::deleteSand(int x, int y) {
  if (this->getPixel(x, y) != 0) {
    this->pixels[y * this->width + x] = 0;
    for (int dx = std::max(0, x - 1); dx <= std::min(this->width - 1, x + 1);
         dx++) {
      for (int dy = std::max(0, y - 1); dy <= std::min(this->height - 1, y + 1);
           dy++) {
        this->cm.setDirty(dx, dy);
      }
      return true;
    }
  }
  return false;
}

Uint32 Sand::getPixel(int x, int y) {
  return this->pixels[y * this->width + x];
}

void Sand::clearBoard() {
  this->clearRequested = true;
#ifndef SINGLETHREAD
  while (this->clearRequested) {
  } // sit and spin until the board is cleared
#endif
}

void Sand::processChunk(Chunk *c, XorShift32 &rng, int *candidates) {
  int num_candidates = 0;
  if (c->dirty) {
    int startX = invertX ? c->endX - 1 : c->x;
    int endX = invertX ? c->x - 1 : c->endX;

    int startY = invertY ? c->y : c->endY - 1;
    int endY = invertY ? c->endY : c->y - 1;

    int dy = this->isGravityInverted ? -1 : 1;
    int stepX = (startX < endX) ? 1 : -1;
    int stepY = (startY < endY) ? 1 : -1;
    bool hasSand = false;
    for (int y = startY; y != endY; y += stepY) {
      int currentRow = y * this->width;
      for (int x = startX; x != endX; x += stepX) {
        Uint32 currentColor = this->pixels[currentRow + x];
        if (currentColor != 0) {
          int nextRow = (y + dy) * this->width;
          num_candidates = 0;
          hasSand = true;
          if (y + dy < 0 || y + dy >= this->height) {
            this->nextFrame[currentRow + x] = currentColor;
            continue;
          }

          if (this->pixels[nextRow + x] == 0 &&
              this->nextFrame[nextRow + x] == 0) {
            candidates[num_candidates++] = x;
          }

          if (x > 0 && this->pixels[nextRow + x - 1] == 0 &&
              this->nextFrame[nextRow + x - 1] == 0) {
            candidates[num_candidates++] = x - 1;
          }
          if (x < this->width - 1 && this->pixels[nextRow + x + 1] == 0 &&
              this->nextFrame[nextRow + x + 1] == 0) {
            candidates[num_candidates++] = x + 1;
          }

          if (num_candidates > 0) {
            int nx = candidates[rng.next() % num_candidates];
            this->nextFrame[nx + nextRow] = currentColor;

            if (y - dy >= 0 && y - dy < this->height) {
              for (int dx = -1; dx <= 1; dx++) {
                if (x + dx > 0 && x + dx < this->width &&
                    this->pixels[x + dx + this->width * (y - dy)])
                  this->cm.setDirty(x + dx, y - dy);
              }
            }
            this->cm.setDirty(x, y);
            this->cm.setDirty(nx, y + dy);
          } else {
            this->nextFrame[currentRow + x] = currentColor;
          }
        }
      }
    }
    c->isEmpty = !hasSand;
  } else if (!c->isEmpty) {
    for (int y = c->y; y < c->endY; y++) {
      int rowOffset = y * this->width;
      auto *srcRow = &this->pixels[rowOffset + c->x];
      auto *dstRow = &this->nextFrame[rowOffset + c->x];
      int rowLength = c->endX - c->x;
      std::memcpy(dstRow, srcRow, rowLength * sizeof(pixels[0]));
    }
  }
}

#ifndef SINGLETHREAD

void Sand::runThread() {
  int candidates[3];
  XorShift32 rng;
  while (this->width != 0) {
    while (true) {
      Chunk *c = this->cm.getNextChunk();
      if (c == NULL)
        break;
      processChunk(c, rng, candidates);
    }
    this->barrier->arrive_and_wait();
  }
}

#else

void Sand::run() {
  int candidates[3];
  while (true) {
    Chunk *c = this->cm.getNextChunk();
    if (c == NULL) {
      if (this->cm.hasNextPhase()) {
        this->cm.startNextPhase();
        continue;
      } else {
        break;
      }
    }
    processChunk(c, this->rng, candidates);
  }
  this->onFinishFrame();
}

#endif
void Sand::setDimensions(int newW, int newH) {
  this->frameLock.lock();
  pendingH = newH;
  pendingW = newW;
  resizeRequested = true;
  this->frameLock.unlock();
#ifndef SINGLETHREAD
  /*while (resizeRequested) { Sit and spin until the resize is finished
  }*/
#endif
}

void Sand::invertGravity() {
  this->isGravityInverted = !this->isGravityInverted;
  this->cm.forceBoardRecheck();
}

int Sand::getWidth() { return this->width; }

int Sand::getHeight() { return this->height; }