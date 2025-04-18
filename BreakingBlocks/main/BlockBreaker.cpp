#include "BlockBreaker.h"
#include "GameManager.h"
#include "GameTypes.h"


BlockBreaker::Ball::Ball(int startX, int startY, int startDX, int startDY, int r) {
  x = startX; y = startY; dx = startDX; dy = startDY; radius = r;
}

void BlockBreaker::Ball::move() {
  x += dx; y += dy;
  if (x <= 0 || x >= 320) dx = -dx;
  if (y <= 0) dy = -dy;
}

void BlockBreaker::Ball::draw() {
  M5.Lcd.fillCircle(x, y, radius, WHITE);
}

void BlockBreaker::Ball::clear() {
  M5.Lcd.fillCircle(x, y, radius, BLACK);
}

void BlockBreaker::Ball::reset(int startX, int startY, int startDX, int startDY) {
  x = startX; y = startY; dx = startDX; dy = startDY;
}

BlockBreaker::Block::Block() {
  visible = false;
}

BlockBreaker::Block::Block(int startX, int startY, int w, int h, uint16_t c) {
  x = startX; y = startY; width = w; height = h; color = c; visible = true;
}

void BlockBreaker::Block::draw() {
  if (visible) M5.Lcd.fillRect(x, y, width, height, color);
}

void BlockBreaker::Block::clear() {
  M5.Lcd.fillRect(x, y, width, height, BLACK);
}

void BlockBreaker::Block::checkCollision(Ball& ball) {
  if (visible && ball.x >= x && ball.x <= x + width && ball.y >= y && ball.y <= y + height) {
    visible = false;
    clear();
    ball.dy = -ball.dy;
    M5.Speaker.tone(1500, 100);
  }
}

void BlockBreaker::setup() {
  M5.Lcd.fillScreen(BLACK);
  for (int row = 0; row < BLOCK_ROWS; row++) {
    for (int col = 0; col < BLOCK_COLS; col++) {
      uint16_t color = colors[random(0, 6)];
      blocks[row][col] = new Block(col * BLOCK_WIDTH, row * BLOCK_HEIGHT + 20, BLOCK_WIDTH, BLOCK_HEIGHT, color);
      blocks[row][col]->draw();
    }
  }
  paddleX = 160;
  ball = new Ball(160, 210, random(-8, 8), random(-7, -1), 5);
  ball->draw();
}

void BlockBreaker::update(GameState& state, bool& scoreDisplayed) {
  int val = analogRead(36);
  val = map(val, 150, 3100, 0, 320 - PADDLE_WIDTH);
  M5.Lcd.fillRect(paddleX, paddleY, PADDLE_WIDTH, 1, BLACK);
  paddleX = val;
  if (M5.BtnC.isPressed()) paddleX += 10;
  if (M5.BtnA.isPressed()) paddleX -= 10;
  paddleX = constrain(paddleX, 0, 320 - PADDLE_WIDTH);
  M5.Lcd.fillRect(paddleX, paddleY, PADDLE_WIDTH, 1, WHITE);

  ball->clear();
  ball->move();

  if (ball->y >= paddleY - ball->radius && ball->y <= paddleY + 10 &&
      ball->x >= paddleX && ball->x <= paddleX + PADDLE_WIDTH) {
    ball->dy = -ball->dy;
    int paddleCenter = paddleX + PADDLE_WIDTH / 2;
    int distanceFromCenter = ball->x - paddleCenter;
    ball->dx = distanceFromCenter / 5;
    M5.Speaker.tone(500, 100);
  }

  for (int row = 0; row < BLOCK_ROWS; row++) {
    for (int col = 0; col < BLOCK_COLS; col++) {
      blocks[row][col]->checkCollision(*ball);
    }
  }

  if (ball->y >= 240) {
    displayGameOver(state, scoreDisplayed);
    return;
  }

  if (allBlocksCleared()) {
    displayGameClear(state, scoreDisplayed);
    return;
  }

  ball->draw();
  delay(30);
}

bool BlockBreaker::allBlocksCleared() {
  for (int row = 0; row < BLOCK_ROWS; row++) {
    for (int col = 0; col < BLOCK_COLS; col++) {
      if (blocks[row][col]->visible) return false;
    }
  }
  return true;
}

void BlockBreaker::displayGameClear(GameState& state, bool& scoreDisplayed) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(60, 120);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.println("GAME CLEAR");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(30, 200);
  M5.Lcd.println("Press any button");
  M5.Lcd.setCursor(30, 220);
  M5.Lcd.println("to return to menu");
  scoreDisplayed = true;
  state = MENU;
}

void BlockBreaker::displayGameOver(GameState& state, bool& scoreDisplayed) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(60, 120);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.println("GAME OVER");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(30, 200);
  M5.Lcd.println("Press any button");
  M5.Lcd.setCursor(30, 220);
  M5.Lcd.println("to return to menu");
  scoreDisplayed = true;
  state = MENU;
}
