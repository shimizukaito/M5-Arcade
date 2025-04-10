#include <M5Stack.h>

#define BLOCK_WIDTH 40
#define BLOCK_HEIGHT 20
#define BLOCK_ROWS 4
#define BLOCK_COLS 8
#define PADDLE_WIDTH 80  // パドルの幅を変数として定義

class Ball {
  public:
    int x, y;
    int dx, dy;
    int radius;

    Ball(int startX, int startY, int startDX, int startDY, int r) {
      x = startX;
      y = startY;
      dx = startDX;
      dy = startDY;
      radius = r;
    }

    void move() {
      x += dx;
      y += dy;

      // 壁に当たった場合の反射
      if (x <= 0 || x >= 320) {
        dx = -dx;
      }
      if (y <= 0) {
        dy = -dy;
      }
    }

    void draw() {
      M5.Lcd.fillCircle(x, y, radius, WHITE);
    }

    void clear() {
      M5.Lcd.fillCircle(x, y, radius, BLACK);
    }

    void reset(int startX, int startY, int startDX, int startDY) {
      x = startX;
      y = startY;
      dx = startDX;
      dy = startDY;
    }
};

class Block {
  public:
    int x, y;
    int width, height;
    uint16_t color;
    bool visible;

    Block() {
      visible = false;
    }

    Block(int startX, int startY, int w, int h, uint16_t c) {
      x = startX;
      y = startY;
      width = w;
      height = h;
      color = c;
      visible = true;
    }

    void draw() {
      if (visible) {
        M5.Lcd.fillRect(x, y, width, height, color);
      }
    }

    void clear() {
      M5.Lcd.fillRect(x, y, width, height, BLACK);
    }

    void checkCollision(Ball &ball) {
      if (visible && ball.x >= x && ball.x <= x + width && ball.y >= y && ball.y <= y + height) {
        visible = false;
        clear();
        ball.dy = -ball.dy;

        // ブロックに当たったときに効果音を再生
        M5.Speaker.tone(1500, 100);  // 1500 Hzのトーンを100ミリ秒再生
      }
    }
};

Ball ball(160, 210, random(-5, 5), -10, 5);
Block blocks[BLOCK_ROWS][BLOCK_COLS];
int paddle_x = 160;
int paddle_y = 220;
bool gameOver = false;

uint16_t colors[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};

void setupGame() {
  M5.Lcd.fillScreen(BLACK);

  // ブロックの初期化と描画
  for (int row = 0; row < BLOCK_ROWS; row++) {
    for (int col = 0; col < BLOCK_COLS; col++) {
      uint16_t color = colors[random(0, 6)];
      blocks[row][col] = Block(col * BLOCK_WIDTH, row * BLOCK_HEIGHT + 20, BLOCK_WIDTH, BLOCK_HEIGHT, color);
      blocks[row][col].draw();
    }
  }

  // パドルの初期描画
  M5.Lcd.fillRect(paddle_x, paddle_y, PADDLE_WIDTH, 1, WHITE);

  // ボールの初期化
  ball.reset(160, 210, random(-8, 8), random(-7, -1));

  // ボールの初期描画
  ball.draw();
}

bool allBlocksCleared() {
  for (int row = 0; row < BLOCK_ROWS; row++) {
    for (int col = 0; col < BLOCK_COLS; col++) {
      if (blocks[row][col].visible) {
        return false; // 少なくとも1つのブロックがまだ表示されている
      }
    }
  }
  return true; // すべてのブロックが消えた
}

void displayGameClear() {
  M5.Lcd.fillScreen(BLACK);
  int textX = 320;

  while (true) {
    M5.Lcd.setCursor(textX, 80);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(4);
    M5.Lcd.print("GAME CLEAR");

    delay(100);
    M5.Lcd.fillRect(textX, 120, 160, 32, BLACK);

    M5.Lcd.setCursor(0, 200);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("Press Button to Restart");

    M5.update();
    if (M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed()) {
      gameOver = false;
      setupGame(); // ゲームをリスタート
      break;
    }
  }
}

void displayGameOver() {
  M5.Lcd.fillScreen(BLACK);
  int textX = 320;

  while (true) {
    M5.Lcd.setCursor(textX, 80);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(4);
    M5.Lcd.print("GAME OVER");

    delay(100);
    M5.Lcd.fillRect(textX, 120, 160, 32, BLACK);

    M5.Lcd.setCursor(0, 200);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("Press Button to Restart");

    M5.update();
    if (M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed()) {
      gameOver = false;
      setupGame(); // ゲームをリスタート
      break;
    }
  }
}

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  pinMode(36, INPUT);
  setupGame(); // ゲームを初期化
  Serial.begin(9600);
}

void loop() {
  int val = analogRead(36);  // 0~4095
  val = map(val, 150, 3100, 0, 320 - PADDLE_WIDTH); // 320からパドルの幅を引く
  Serial.println(val);

  if (gameOver) {
    displayGameOver();
    return;
  }

  M5.update();

  // パドルの位置を消去
  M5.Lcd.fillRect(paddle_x, paddle_y, PADDLE_WIDTH, 1, BLACK);

  paddle_x = val;

  if (M5.BtnC.isPressed()) {
    paddle_x += 10;  // 左に移動
  }
  if (M5.BtnA.isPressed()) {
    paddle_x -= 10;  // 右に移動
  }
  Serial.println(val);

  // 画面外に出ないように制限
  if (paddle_x < 0) {
    paddle_x = 0;
  }
  if (paddle_x > 320 - PADDLE_WIDTH) { // 320 - パドルの幅
    paddle_x = 320 - PADDLE_WIDTH;
  }

  // パドルを再描画
  M5.Lcd.fillRect(paddle_x, paddle_y, PADDLE_WIDTH, 1, WHITE);

  // ボールの描画と移動
  ball.clear();
  ball.move();

  // パドルに当たった場合の反射
  // パドルに当たった場合の反射
  if (ball.y >= paddle_y - ball.radius && ball.y <= paddle_y + 10) {
    if (ball.x >= paddle_x && ball.x <= paddle_x + PADDLE_WIDTH) {
      ball.dy = -ball.dy;

      // パドルに当たったときに効果音を再生
      M5.Speaker.tone(500, 100);  // 1000 Hzのトーンを100ミリ秒再生

      // パドルの中央からの距離に応じてボールのdxを調整
      int paddleCenter = paddle_x + PADDLE_WIDTH / 2;
      int distanceFromCenter = ball.x - paddleCenter;
      ball.dx = distanceFromCenter / 5;
    }
  }

  // ブロックに当たった場合の反射
  for (int row = 0; row < BLOCK_ROWS; row++) {
    for (int col = 0; col < BLOCK_COLS; col++) {
      blocks[row][col].checkCollision(ball);
    }
  }

  // 底に到達した場合、ゲームオーバー状態に移行
  if (ball.y >= 240) {
    gameOver = true;
  }

  // ゲームクリアをチェック
  if (allBlocksCleared()) {
    displayGameClear();
    return;
  }

  ball.draw();

  delay(30);
}
