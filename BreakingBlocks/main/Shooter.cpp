#include "Shooter.h"
#include "GameTypes.h"
#include <TimeLib.h>

void Shooter::setup() {
  gameStartTime = millis();
  score = 0;
  bulletCount = 0;
  enemyCount = 0;
  shipX = 160;
  M5.Lcd.fillScreen(BLACK);
  drawScore();
}

void Shooter::update(GameState& state, bool& scoreDisplayed) {
  if (millis() - gameStartTime >= 15000) {
    displayFinalScore(state, scoreDisplayed);
    return;
  }

  drawScore();
  drawClock();

  static int lastShipX = -1;
  static unsigned long lastShotTime = 0;
  static unsigned long lastEnemyTime = 0;

  int val = analogRead(36);
  shipX = map(val, 150, 3100, shipSize, 320 - shipSize);
  if (shipX != lastShipX) {
    drawShip(lastShipX, shipY, shipSize, BLACK);
    drawShip(shipX, shipY, shipSize, WHITE);
    lastShipX = shipX;
  }

  if (millis() - lastShotTime > 200) {
    shootBullet(shipX, shipY - shipSize);
    lastShotTime = millis();
  }

  if (millis() - lastEnemyTime > 1000) {
    spawnEnemy();
    lastEnemyTime = millis();
  }

  updateBullets();
  updateEnemies();
}

void Shooter::drawShip(int x, int y, int size, uint16_t color) {
  if (x < 0) return;
  M5.Lcd.fillTriangle(x, y - size, x - size, y + size, x + size, y + size, color);
}

void Shooter::shootBullet(int x, int y) {
  if (bulletCount < MAX_BULLETS) {
    bullets[bulletCount].x = x;
    bullets[bulletCount].y = y;
    bullets[bulletCount].speed = -8;
    bulletCount++;
  }
}

void Shooter::updateBullets() {
  for (int i = 0; i < bulletCount; i++) {
    M5.Lcd.fillRect(bullets[i].x - 1, bullets[i].y - 4, 3, 6, BLACK);
    bullets[i].y += bullets[i].speed;
    if (bullets[i].y < 0) {
      bullets[i] = bullets[--bulletCount];
      i--;
      continue;
    }
    M5.Lcd.fillRect(bullets[i].x - 1, bullets[i].y - 4, 3, 6, WHITE);
  }
}

void Shooter::drawScore() {
  M5.Lcd.fillRect(0, 0, 160, 20, BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Score: ");
  M5.Lcd.print(score);
}

void Shooter::drawClock() {
  unsigned long remaining = 15000 - (millis() - gameStartTime);
  if ((int)remaining < 0) remaining = 0;
  unsigned long ms = remaining / 1000;
  int h = ms / 3600;
  int m = (ms % 3600) / 60;
  int s = ms % 60;
  M5.Lcd.fillRect(240, 0, 80, 20, BLACK);
  M5.Lcd.setCursor(280, 0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("%02d", s);
  int barMaxWidth = 320;
  int barHeight = 6;
  int filledWidth = map(remaining, 0, 15000, 0, barMaxWidth);
  M5.Lcd.fillRect(0, 234, barMaxWidth, barHeight, DARKGREY);
  M5.Lcd.fillRect(0, 234, filledWidth, barHeight, GREEN);
}

void Shooter::spawnEnemy() {
  if (enemyCount >= MAX_ENEMIES) return;
  Enemy e;
  int side = random(0, 2);
  int minY = 20;
  int maxY = shipY - shipSize * 3;
  if (maxY <= minY) maxY = minY + 1;
  if (side == 0) {
    e.x = -10; e.y = random(minY, maxY);
    e.vx = 1;
    e.vy = 0;
  } else {
    e.x = 330;
    e.y = random(minY, maxY);
    e.vx = -1;
    e.vy = 0;
  }
  e.angle = 0;
  e.alive = true;
  enemies[enemyCount++] = e;
}

void Shooter::updateEnemies() {
  for (int i = 0; i < enemyCount; i++) {
    if (!enemies[i].alive) {
      M5.Lcd.fillCircle(enemies[i].x, enemies[i].y, 6, BLACK);
      enemies[i] = enemies[--enemyCount];
      i--;
      continue;
    }
    M5.Lcd.fillCircle(enemies[i].x, enemies[i].y, 6, BLACK);
    enemies[i].angle += 0.1;
    enemies[i].x += enemies[i].vx;
    enemies[i].y += enemies[i].vy + sin(enemies[i].angle) * 1.5;
    for (int j = 0; j < bulletCount; j++) {
      int dx = enemies[i].x - bullets[j].x;
      int dy = enemies[i].y - bullets[j].y;
      if (dx * dx + dy * dy < 36) {
        enemies[i].alive = false;
        M5.Speaker.tone(300, 150);  // 低くて短めの爆発音
        bullets[j] = bullets[--bulletCount];
        score++;
        drawScore();
        break;
      }
    }
    if (enemies[i].x < -10 || enemies[i].x > 330 || enemies[i].y < -10 || enemies[i].y > 250) {
      M5.Lcd.fillCircle(enemies[i].x, enemies[i].y, 6, BLACK);
      enemies[i] = enemies[--enemyCount];
      i--;
      continue;
    }
    if (enemies[i].alive) {
      M5.Lcd.fillCircle(enemies[i].x, enemies[i].y, 6, RED);
    }
  }
}

void Shooter::displayFinalScore(GameState& state, bool& scoreDisplayed) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(150, 80);
  M5.Lcd.setTextSize(8);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.print(score);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(30, 200);
  M5.Lcd.println("Press any button");
  M5.Lcd.setCursor(30, 220);
  M5.Lcd.println("to return to menu");
  scoreDisplayed = true;
  state = MENU;
}
