#pragma once
#include "BlockBreaker.h"
#include "Shooter.h"
#include "GameTypes.h"
#include "FaceBG.h"   // ←追加

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

  FaceBG face;        // ←追加

  void showMenu();
};
