#include "chunkManager.hpp"

inline int phaseFor(int cx, int cy) { return (cx & 1) | ((cy & 1) << 1); }
int chunkManager::getNumChunks() { return numChunks; }

void chunkManager::forceBoardRecheck() {
  for (int i = 0; i < numChunks; i++) {
    chunks[i].dirty = true;
    chunks[i].nextDirty = true;
    chunks[i].isEmpty = false;
  }
}

void chunkManager::setDirty(int gridX, int gridY) {
  int cx = gridX / CHUNK_WIDTH;
  int cy = gridY / CHUNK_HEIGHT;

  chunks[cy * chunksX + cx].nextDirty = true;
  chunks[cy * chunksX + cx].dirty = true;
}

Chunk *chunkManager::getChunk(int gridX, int gridY) {
  int cx = gridX / CHUNK_WIDTH;
  int cy = gridY / CHUNK_HEIGHT;
  return &chunks[cy * chunksX + cx];
}

Chunk *chunkManager::getNextChunk() {
  if (currentPhase < 4) {
    auto &vec = phases[currentPhase];
    int cc = currentChunk.fetch_add(1);
    if (cc < this->phases[currentPhase].size()) {
      return vec[cc];
    }
    return NULL;
  }
  return NULL;
}

bool chunkManager::startNextPhase() {

  currentChunk = 0;
  currentPhase++;
  return currentPhase < 4;
}

bool chunkManager::hasNextPhase() { return currentPhase < 4; }

void chunkManager::startCycle() {
  currentPhase = 0;
  currentChunk = 0;

  for (int i = 0; i < numChunks; i++) {
    chunks[i].dirty = chunks[i].nextDirty;
    chunks[i].nextDirty = false;
  }
}

void chunkManager::setGridSize(int w, int h) {
  gridWidth = w;
  gridHeight = h;

  chunksX = (gridWidth + CHUNK_WIDTH - 1) / CHUNK_WIDTH;
  chunksY = (gridHeight + CHUNK_HEIGHT - 1) / CHUNK_HEIGHT;
  numChunks = chunksX * chunksY;

  if (chunks != NULL)
    delete[] chunks;
  chunks = new Chunk[numChunks];

  for (int i = 0; i < 4; i++)
    phases[i].clear();

  int index = 0;
  for (int cy = 0; cy < chunksY; cy++) {
    for (int cx = 0; cx < chunksX; cx++) {
      Chunk &c = chunks[index++];
      c.x = cx * CHUNK_WIDTH;
      c.y = cy * CHUNK_HEIGHT;
      c.endX = std::min(c.x + CHUNK_WIDTH, gridWidth);
      c.endY = std::min(c.y + CHUNK_HEIGHT, gridHeight);
      c.dirty = true;
      c.nextDirty = false;
      c.isEmpty = false;

      int phase = phaseFor(cx, cy);
      phases[phase].push_back(&c);
    }
  }

  startCycle();
}

chunkManager::~chunkManager() { delete[] chunks; }

chunkManager::chunkManager(int gridWidth, int gridHeight) {
  setGridSize(gridWidth, gridHeight);
}

Chunk *chunkManager::getChunks() { return this->chunks; }