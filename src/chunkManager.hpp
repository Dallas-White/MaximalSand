
#include <atomic>
#include <vector>
const int CHUNK_WIDTH = 64;
const int CHUNK_HEIGHT = 64;

struct Chunk {
  int x;
  int y;
  int endX;
  int endY;
  bool dirty;
  bool nextDirty;
  bool isEmpty;
};

class chunkManager {
private:
  std::vector<Chunk *> phases[4];
  Chunk *chunks = NULL;
  int chunksX;
  int chunksY;
  int gridWidth;
  int gridHeight;
  int numChunks;
  int currentPhase;
  std::atomic<int> currentChunk;

public:
  chunkManager(int gridWidth, int gridHeight);
  ~chunkManager();
  void setGridSize(int gridWidth, int gridHeight);
  bool hasNextPhase();
  Chunk *getNextChunk();
  bool startNextPhase();
  void startCycle();
  int getNumChunks();
  Chunk *getChunks();
  void forceBoardRecheck();
  Chunk *getChunk(int gridX, int gridY);
  void setDirty(int gridX, int gridY);
};