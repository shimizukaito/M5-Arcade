#pragma once
#include "BlockBreaker.h"
#include "Shooter.h"
#include "GameTypes.h"

class Shooter;

class GameManager {
public:
  void init();
  void update();

private:
  GameState currentGame = MENU;
  BlockBreaker blockBreaker;
  Shooter shooter;
  bool scoreDisplayActive = false;

  void showMenu();
};
