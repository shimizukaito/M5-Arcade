#include "FaceBG.h"

// ---------- utils ----------
float FaceBG::clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }
float FaceBG::lerpf(float a, float b, float t) { return a + (b - a) * t; }
float FaceBG::easeInOut(float t) {
  t = clampf(t, 0.0f, 1.0f);
  return t * t * (3.0f - 2.0f * t); // smoothstep
}
float FaceBG::mapf(float v, float inMin, float inMax, float outMin, float outMax) {
  if (inMax == inMin) return outMin;
  float t = (v - inMin) / (inMax - inMin);
  t = clampf(t, 0.0f, 1.0f);
  return outMin + (outMax - outMin) * t;
}

// ---------- scheduling ----------
void FaceBG::scheduleAutoGaze() {
  nextGazeMs = millis() + random(900, 2200);
  autoTX = random(-60, 61) / 100.0f; // -0.6..0.6
  autoTY = random(-30, 31) / 100.0f; // -0.3..0.3
}
void FaceBG::scheduleBlink() { nextBlinkMs = millis() + random(2200, 6000); }
void FaceBG::startBlink() { blinking = true; blinkStartMs = millis(); }

// 0..1（0=open, 1=closed）
float FaceBG::blinkValue() {
  if (!blinking) return 0.0f;

  // まぶたアニメ（ゆっくり）
  const uint32_t closeTime = 220;
  const uint32_t holdTime  = 180;
  const uint32_t openTime  = 260;
  const uint32_t total     = closeTime + holdTime + openTime;

  uint32_t t = millis() - blinkStartMs;
  if (t >= total) {
    blinking = false;
    scheduleBlink();
    return 0.0f;
  }

  if (t < closeTime) return easeInOut((float)t / closeTime);
  if (t < closeTime + holdTime) return 1.0f;

  uint32_t tt = t - (closeTime + holdTime);
  return 1.0f - easeInOut((float)tt / openTime);
}

// ---------- pot ----------
bool FaceBG::potIsActive(uint32_t now) const {
  return (now - lastPotMoveMs) < POT_ACTIVE_HOLD_MS;
}

void FaceBG::updatePot(uint32_t now) {
  potRaw = analogRead(POT_PIN);

  if (abs(potRaw - potLastRaw) >= POT_MOVE_DELTA) {
    lastPotMoveMs = now;
    potLastRaw = potRaw;
  }

  // -1..1
  float x = mapf((float)potRaw, (float)POT_MIN, (float)POT_MAX, -1.0f, 1.0f);

  // 少し落ち着かせる（ノイズ吸収）
  static float xf = 0.0f;
  xf = lerpf(xf, x, 0.18f);
  manualX = xf;
}

// ---------- drawing ----------
void FaceBG::drawStaticFaceOnce() {
  // ここで背景を作る（メニューUIはこの後に上書き描画）
  M5.Lcd.fillScreen(BG);

  // 口（固定：長方形バー）
  int mx = CX - MOUTH_W / 2;
  int my = MOUTH_Y - MOUTH_H / 2;
  M5.Lcd.fillRect(mx, my, MOUTH_W, MOUTH_H, FG);
}

void FaceBG::clearEyeBox(int ex, int ey) {
  int x = ex - EYE_BOX_W / 2;
  int y = ey - EYE_BOX_H / 2;
  M5.Lcd.fillRect(x, y, EYE_BOX_W, EYE_BOX_H, BG);
}

void FaceBG::drawEyeWithLids(int ex, int ey, int lidPx) {
  // eye dot
  M5.Lcd.fillCircle(ex, ey, EYE_R, FG);

  if (lidPx <= 0) return;

  int top = ey - EYE_BOX_H / 2;
  int bot = ey + EYE_BOX_H / 2;

  // eyelids
  M5.Lcd.fillRect(ex - EYE_BOX_W / 2, top, EYE_BOX_W, lidPx, BG);
  M5.Lcd.fillRect(ex - EYE_BOX_W / 2, bot - lidPx, EYE_BOX_W, lidPx, BG);

  // fully closed line
  if (lidPx >= (EYE_BOX_H / 2 - 2)) {
    M5.Lcd.drawLine(ex - 12, ey, ex + 12, ey, FG);
    M5.Lcd.drawLine(ex - 12, ey + 1, ex + 12, ey + 1, FG);
  }
}

// ---------- public ----------
void FaceBG::begin() {
  randomSeed(esp_random());
  analogReadResolution(12);

  // 初期化
  drawStaticFaceOnce();
  scheduleAutoGaze();
  scheduleBlink();

  potLastRaw = analogRead(POT_PIN);
  lastPotMoveMs = 0;

  // 初回描画（目を中央に）
  prevLx = prevLy = prevRx = prevRy = -999;
  prevLidPx = -1;

  lastFrameMs = millis();
}

void FaceBG::update() {
  // 30fps
  uint32_t now = millis();
  if (now - lastFrameMs < 33) return;
  lastFrameMs = now;

  updatePot(now);
  bool active = potIsActive(now);

  // auto gaze
  if (now >= nextGazeMs) scheduleAutoGaze();
  autoX = lerpf(autoX, autoTX, 0.04f);
  autoY = lerpf(autoY, autoTY, 0.04f);

  // blend: 操作中はmanualX、操作してない時はauto（pot値は無視）
  float targetX = active ? manualX : autoX;
  gazeX = lerpf(gazeX, targetX, active ? 0.25f : 0.06f);
  gazeY = autoY;

  // blink
  if (!blinking && now >= nextBlinkMs) startBlink();
  float b = blinkValue();
  int lidPx = (int)(b * (EYE_BOX_H / 2 + 4));

  // eye positions（移動量はここで調整）
  const float MOVE_X = 25.0f; // ←もっと動かしたければ 22, 26 などへ
  const float MOVE_Y = 6.0f;
  int ox = (int)(gazeX * MOVE_X);
  int oy = (int)(gazeY * MOVE_Y);

  int lx = CX - EYE_SPAN_X + ox;
  int ly = EYE_BASE_Y + oy;
  int rx = CX + EYE_SPAN_X + ox;
  int ry = EYE_BASE_Y + oy;

  bool changed =
    (lx != prevLx) || (ly != prevLy) ||
    (rx != prevRx) || (ry != prevRy) ||
    (lidPx != prevLidPx);

  if (!changed) return;

  // erase old
  if (prevLx > -500) clearEyeBox(prevLx, prevLy);
  if (prevRx > -500) clearEyeBox(prevRx, prevRy);

  // draw new
  clearEyeBox(lx, ly);
  clearEyeBox(rx, ry);
  drawEyeWithLids(lx, ly, lidPx);
  drawEyeWithLids(rx, ry, lidPx);

  prevLx = lx; prevLy = ly;
  prevRx = rx; prevRy = ry;
  prevLidPx = lidPx;
}
