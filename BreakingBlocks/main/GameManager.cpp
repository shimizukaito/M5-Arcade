#include <M5Stack.h>
#include "GameManager.h"
#include "GameTypes.h"

void GameManager::init() {
  showMenu();
}

void GameManager::showMenu() {
  // 1) 背景として顔を描く（ここで黒背景も作る）
  face.begin();

  // 2) その上に既存のメニューUIを“そのまま”描く（座標も文字も同じ）
  M5.Lcd.setCursor(30, 100);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(30, 150);
  M5.Lcd.println("Block");

  M5.Lcd.setCursor(130, 150);
  M5.Lcd.println("Shooter");

  M5.Lcd.setCursor(60, 200);
  M5.Lcd.println("V");

  M5.Lcd.setCursor(160, 200);
  M5.Lcd.println("V");
}

void GameManager::update() {
  if (scoreDisplayActive) {
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
      currentGame = MENU;
      scoreDisplayActive = false;
      showMenu();
    }
    return;
  }

  if (currentGame == MENU) {
    // ★メニューでも可変抵抗で目を動かす＆操作してない時は自動視線
    // 差分更新なのでUIは基本そのまま
    face.update();

    if (M5.BtnA.wasPressed()) {
      currentGame = BLOCK_BREAKER;
      blockBreaker.setup();
    } else if (M5.BtnB.wasPressed()) {
      currentGame = SHOOTER;
      shooter.setup();
    }
    return;
  }

  if (currentGame == BLOCK_BREAKER) {
    blockBreaker.update(currentGame, scoreDisplayActive);
  } else if (currentGame == SHOOTER) {
    shooter.update(currentGame, scoreDisplayActive);
  }
}
