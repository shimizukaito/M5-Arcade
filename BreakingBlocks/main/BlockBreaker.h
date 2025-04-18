#pragma once
#include <M5Stack.h>
#include "GameTypes.h"

class BlockBreaker {
public:
  void setup();
  void update(GameState& state, bool& scoreDisplayed);

private:
  static const int BLOCK_WIDTH = 40;
  static const int BLOCK_HEIGHT = 20;
  static const int BLOCK_ROWS = 4;
  static const int BLOCK_COLS = 8;
  static const int PADDLE_WIDTH = 80;

  class Ball {
  public:
    int x, y, dx, dy, radius;
    Ball(int startX, int startY, int startDX, int startDY, int r);
    void move();
    void draw();
    void clear();
    void reset(int startX, int startY, int startDX, int startDY);
  };

  class Block {
  public:
    int x, y, width, height;
    uint16_t color;
    bool visible;
    Block();
    Block(int startX, int startY, int w, int h, uint16_t c);
    void draw();
    void clear();
    void checkCollision(Ball& ball);
  };

  Ball* ball;
  Block* blocks[BLOCK_ROWS][BLOCK_COLS];
  int paddleX = 160;
  int paddleY = 220;
  uint16_t colors[6] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};

  bool allBlocksCleared();
  void displayGameClear(GameState& state, bool& scoreDisplayed);
  void displayGameOver(GameState& state, bool& scoreDisplayed);
};
