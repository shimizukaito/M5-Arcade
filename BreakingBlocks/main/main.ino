#include <M5Stack.h>
#include "GameManager.h"

GameManager game;

void setup() {
  M5.begin();
  pinMode(36, INPUT);
  game.init();
}

void loop() {
  M5.update();
  game.update();
}
