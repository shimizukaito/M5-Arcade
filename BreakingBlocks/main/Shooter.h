#pragma once
#include <M5Stack.h>
#include "GameTypes.h"

class Shooter {
public:
  void setup();
  void update(GameState& state, bool& scoreDisplayed);

private:
  static const int MAX_BULLETS = 10;
  static const int MAX_ENEMIES = 10;

  struct Bullet {
    int x, y, speed = -8;
  };
  Bullet bullets[MAX_BULLETS];
  int bulletCount = 0;

  struct Enemy {
    float x, y, vx, vy, angle;
    bool alive;
  };
  Enemy enemies[MAX_ENEMIES];
  int enemyCount = 0;

  int shipX = 160;
  int shipY = 200;
  int shipSize = 10;

  int score = 0;
  unsigned long gameStartTime = 0;

  void drawShip(int x, int y, int size, uint16_t color);
  void shootBullet(int x, int y);
  void updateBullets();
  void drawScore();
  void drawClock();
  void spawnEnemy();
  void updateEnemies();
  void displayFinalScore(GameState& state, bool& scoreDisplayed);
};
