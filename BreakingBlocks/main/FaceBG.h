#pragma once
#include <M5Stack.h>

class FaceBG {
public:
  void begin();   // メニュー表示時に1回呼ぶ（背景+口+最初の目）
  void update();  // MENU中に毎フレーム呼ぶ（目だけ差分更新）

private:
  // ===== レイアウト（UIに当たらないよう上寄せ）=====
  static const int CX = 160;
  static const int EYE_BASE_Y = 55;   // 目は上側へ
  static const int MOUTH_Y    = 100;  // 口も上側へ（MENU文字 y=150 以降を避ける）

  // ===== 見た目 =====
  static const uint16_t BG = BLACK;
  static const uint16_t FG = WHITE;

  static const int EYE_SPAN_X = 70;
  static const int EYE_R = 6;

  // 目の差分更新範囲（ここだけ黒で消して描き直す）
  static const int EYE_BOX_W = 60;
  static const int EYE_BOX_H = 46;

  // 口（固定の横棒：MENUでは動かさない）
  static const int MOUTH_W = 90;
  static const int MOUTH_H = 10;

  // ===== 可変抵抗（GPIO36）=====
  static const int POT_PIN = 36;
  static const int POT_MIN = 150;
  static const int POT_MAX = 3100;
  static const int POT_MOVE_DELTA = 20;           // ノイズで誤判定しやすければ増やす
  static const uint32_t POT_ACTIVE_HOLD_MS = 800; // 最後に動いた後、操作中扱いする時間

  // ===== アニメ =====
  uint32_t lastFrameMs = 0;

  // 自動視線（操作してない時）
  float autoX = 0.0f, autoY = 0.0f;
  float autoTX = 0.0f, autoTY = 0.0f;
  uint32_t nextGazeMs = 0;

  // 可変抵抗（操作中）
  int potRaw = 0;
  int potLastRaw = 0;
  uint32_t lastPotMoveMs = 0;
  float manualX = 0.0f;   // -1..1

  // ブレンド後の最終視線
  float gazeX = 0.0f;
  float gazeY = 0.0f;

  // まばたき
  bool blinking = false;
  uint32_t nextBlinkMs = 0;
  uint32_t blinkStartMs = 0;

  // 前回描画（消すため）
  int prevLx = -999, prevLy = -999;
  int prevRx = -999, prevRy = -999;
  int prevLidPx = -1;

  // utils
  static float clampf(float v, float a, float b);
  static float lerpf(float a, float b, float t);
  static float easeInOut(float t);
  static float mapf(float v, float inMin, float inMax, float outMin, float outMax);

  void scheduleAutoGaze();
  void scheduleBlink();
  void startBlink();
  float blinkValue(); // 0..1（0=open, 1=closed）

  void drawStaticFaceOnce();
  void clearEyeBox(int ex, int ey);
  void drawEyeWithLids(int ex, int ey, int lidPx);

  void updatePot(uint32_t now);
  bool potIsActive(uint32_t now) const;
};
