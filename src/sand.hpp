
#include <chrono>
#ifndef SAND
#include "chunkManager.hpp"
#include "utils.hpp"
#include <SDL2/SDL.h>
#include <functional>
#include <mutex>
#ifndef SINGLETHREAD
#include <barrier>
#include <thread>
#endif

class Sand {
private:
  Uint32 *pixels;
#ifndef SINGLETHREAD
  std::vector<std::thread> threads;
  std::barrier<std::function<void()>> *barrier;
  void onBarrierFinished();
  std::chrono::time_point<std::chrono::high_resolution_clock> frameStartTime;
#else
  XorShift32 rng;
#endif
  int width;
  int height;
  int frameCounter = 0;
  bool invertX = false;
  bool invertY = false;
  Uint32 *nextFrame;
  std::mutex frameLock;
  bool isGravityInverted;
  std::atomic<bool> clearRequested;
  std::atomic<bool> resizeRequested = false;
  int pendingW, pendingH;
  chunkManager cm;
  void processChunk(Chunk *c, XorShift32 &rng, int *candidates);

public:
  ~Sand();
  void onFinishFrame();
  void invertGravity();
  void clearBoard();
  static bool inBound(int x, int y, int w, int h);
  bool inBound(int x, int y);
  Sand(int w, int h);
#ifndef SINGLETHREAD
  void runThread();
#else
  void run();
#endif
  void draw(Uint32 *buffer, int x, int y, int width, int height,
            int screen_width);
  Uint32 getPixel(int x, int y);
  void createSand(int x, int y, Uint32 color);
  bool deleteSand(int x, int y);
  void setDimensions(int w, int h);
  int getWidth();
  int getHeight();
};
#define SAND
#endif