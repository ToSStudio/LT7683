// SAMD21 with TFT 1024 x 600 (LT7683 / RA8876) using SPI
// Display: ER-TFT101B4-1-5553 available from BUYDISPLAY / Eastrising

// connect the power supply of the Display to GND & 5 Volt.

// This is a Demo to verify wiring and Lib initialized correctly

#include "LT7683.h"

LT7683 tft(5, 9);

// --- primitives panel position (top-left) ---
const uint16_t PW = 260;
const uint16_t PH = 180;
const uint16_t PX = 40;
const uint16_t PY = 40;

// --- 7-seg panel position (top-right) ---
const uint16_t SX = 744;
const uint16_t SY = 20;
const uint16_t SW = PW;
const uint16_t SH = PH;

// sin(deg) * 128, deg = 0..90  (uint8_t, 91 bytes)
static const uint8_t sin90_u8_128[91] PROGMEM = {
  0, 2, 4, 7, 9, 11, 13, 16, 18, 20, 22, 24, 27, 29, 31, 33,
  35, 37, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66,
  68, 70, 72, 73, 75, 77, 79, 81, 82, 84, 86, 87, 89, 91, 92, 94,
  95, 97, 98, 99, 101, 102, 104, 105, 106, 107, 109, 110, 111, 112, 113, 114,
  115, 116, 117, 118, 119, 119, 120, 121, 122, 122, 123, 124, 124, 125, 125, 126,
  126, 126, 127, 127, 127, 128, 128, 128, 128, 128, 128
};

struct Pos {
  int16_t x;
  int16_t y;
};

static const uint8_t BG_R = 8;
static const uint8_t BG_G = 18;
static const uint8_t BG_B = 40;

static const uint8_t FG_R = 230;
static const uint8_t FG_G = 230;
static const uint8_t FG_B = 230;

// Demo Background
void drawBackground() {

  // clear entire screen
  tft.clearScreen();

  tft.fore(220, 200, 80);   // warm yellow, not too loud
tft.roundRect(0, 0, 1023, 599, 6);

  // panel frame (top-left)
  tft.fore(120, 140, 180);  // subtle bluish frame
  tft.roundRect(PX, PY, PX + PW, PY + PH, 10);

  // panel title
  tft.fore(200, 220, 255);
  tft.setCursor(PX + 10, PY + 6);
  tft.print("PRIMITIVES");
}

void drawPrimitivesDemo() {

  // use brighter color for shapes
  tft.fore(255, 200, 120);

  // --- LINE ---
  tft.line(PX + 20, PY + 40, PX + 100, PY + 40);

  // --- RECT ---
  tft.rect(PX + 20, PY + 60, PX + 60, PY + 100);

  // --- FILLED RECT ---
  tft.fillRect(PX + 70, PY + 60, PX + 110, PY + 100);

  // --- CIRCLE ---
  tft.circle(PX + 150, PY + 70, 15);

  // --- FILLED CIRCLE ---
  tft.fillCircle(PX + 200, PY + 70, 15);

  // --- TRIANGLE ---
  tft.triangle(PX + 30, PY + 130,
               PX + 10, PY + 160,
               PX + 50, PY + 160);

  // --- FILLED TRIANGLE ---
  tft.fillTriangle(PX + 90, PY + 130,
                   PX + 70, PY + 160,
                   PX + 110, PY + 160);

  // --- ROUNDED RECT ---
  tft.roundRect(PX + 130, PY + 120, PX + 180, PY + 160, 8);

  // --- FILLED ROUNDED RECT ---
  tft.fillRoundRect(PX + 190, PY + 120, PX + 240, PY + 160, 8);
}


void draw7SegDemo() {

  const uint16_t SX = 744;
  const uint16_t SY = 40;
  const uint16_t SW = 260;
  const uint16_t SH = 180;

  // frame
  tft.fore(120, 140, 180);
  tft.roundRect(SX, SY, SX + SW, SY + SH, 10);

  // title
  tft.fore(200, 220, 255);
  tft.setCursor(SX + 10, SY + 6);
  tft.print("7-SEG");

  // digit color
  tft.fore(80, 220, 255);

  // common visual baseline near bottom of panel
  // larger digits start higher
  tft.set7SegScale(1);
  tft.plot7Seg(SX + 18, SY + 112, 1);

  tft.set7SegScale(2);
  tft.plot7Seg(SX + 50, SY + 88, 2);

  tft.set7SegScale(3);
  tft.plot7Seg(SX + 102, SY + 64, 3);

  tft.set7SegScale(4);
  tft.plot7Seg(SX + 178, SY + 40, 4);
}

void drawTextDemo() {

  // -----------------------------
  // master frame for "TEXT" demo
  // -----------------------------
  const uint16_t TX = 40;
  const uint16_t TY = 370;
  const uint16_t TW = 260;
  const uint16_t TH = 220;

  // -----------------------------
  // derived geometry
  // -----------------------------
  const uint16_t TITLE_X = TX + 10;
  const uint16_t TITLE_Y = TY + 6;

  const uint16_t TEMP_X1 = TX + 120;
  const uint16_t TEMP_Y1 = TY + 10;
  const uint16_t TEMP_X2 = TX + TW - 10;
  const uint16_t TEMP_Y2 = TY + TH - 130;

  // START button tied to same master frame
  const uint16_t BTN_X = TX + 12;
  const uint16_t BTN_Y = TY + 164;
  const uint16_t BTN_W = 120;
  const uint16_t BTN_H = 34;

  // text lines
  const uint16_t L1_X = TX + 12;
  const uint16_t L1_Y = TY + 56;

  const uint16_t L2_X = TX + 12;
  const uint16_t L2_Y = TY + 80;

  const uint16_t L3_X = TX + 12;
  const uint16_t L3_Y = TY + 115;

  const uint16_t TEMP_TX = TX + TW - 110;
  const uint16_t TEMP_TY = TY + 26;

  // -----------------------------
  // frame
  // -----------------------------
  tft.fore(120, 140, 180);
  tft.roundRect(TX, TY, TX + TW, TY + TH, 10);
  tft.roundRect(TEMP_X1, TEMP_Y1, TEMP_X2, TEMP_Y2, 10);  // for "~ 23 °"

  // -----------------------------
  // title
  // -----------------------------
  tft.fore(200, 220, 255);
  tft.textSize(3);
  tft.textScale(1);
  tft.setCursor(TITLE_X, TITLE_Y);
  tft.print("TEXT");

  // -----------------------------
  // START button
  // -----------------------------
  tft.fore(40, 180, 80);
  tft.roundRect(BTN_X, BTN_Y, BTN_X + BTN_W, BTN_Y + BTN_H, 8);

  tft.fore(20, 90, 40);
  tft.fillRect(BTN_X + 2, BTN_Y + 2, BTN_X + BTN_W - 2, BTN_Y + BTN_H - 2);

  tft.fore(200, 255, 200);
  tft.back(20, 90, 40);
  tft.textSize(3);
  tft.textScale(1);
  tft.setCursor(BTN_X + 18, BTN_Y+2);
  tft.print("START");

  // restore normal text background
  tft.back(0, 0, 0);

  // -----------------------------
  // line 1: base text
  // -----------------------------
  tft.fore(255, 220, 120);
  tft.textSize(1);
  tft.textScale(1);
  tft.setCursor(L1_X, L1_Y);
  tft.print("Size 1");

  // -----------------------------
  // line 2: larger font
  // -----------------------------
  tft.fore(0, 255, 0);
  tft.textSize(2);
  tft.textScale(1);
  tft.setCursor(L2_X, L2_Y);
  tft.print("Size 2");

  // -----------------------------
  // line 3: scaled text
  // -----------------------------
  tft.fore(255, 0, 0);
  tft.textSize(1);
  tft.textScale(2);
  tft.setCursor(L3_X, L3_Y);
  tft.print("Scale x2");

  // -----------------------------
  // special chars / temp box
  // -----------------------------
  tft.fore(80, 220, 255);
  tft.textSize(3);
  tft.textScale(1);
  tft.setCursor(TEMP_TX, TEMP_TY);
  tft.print("~ 23"
            "\xB0");
}




void drawBMW(uint8_t state) {

  const uint16_t CX = 880;
  const uint16_t CY = 460;
  const uint16_t R = 39;  // 1 px smaller than outer ring

  // demo colors
  const uint8_t fr = 20;
  const uint8_t fg = 20;
  const uint8_t fb = 230;

  const uint8_t br = 200;  // 6
  const uint8_t bg = 220;  // 18
  const uint8_t bb = 255;  // 40

  if (state == 0) {
    tft.fore(br, bg, bb);
    tft.fillArc(CX, CY, R, R, 0);
    tft.fillArc(CX, CY, R, R, 2);

    tft.fore(fr, fg, fb);
    tft.fillArc(CX, CY, R, R, 1);
    tft.fillArc(CX, CY, R, R, 3);
  } else {
    tft.fore(br, bg, bb);
    tft.fillArc(CX, CY, R, R, 1);
    tft.fillArc(CX, CY, R, R, 3);

    tft.fore(fr, fg, fb);
    tft.fillArc(CX, CY, R, R, 0);
    tft.fillArc(CX, CY, R, R, 2);
  }

  // restore demo foreground
  tft.fore(fr, fg, fb);
}



static void drawBezelStatic() {

  const int16_t CX = 520;
  const int16_t CY = 330;

  const uint16_t R_OUTER = 172;
  const uint16_t R_INNER = 168;
  const uint16_t R_CENTER = 124;

  tft.fore(FG_R, FG_G, FG_B);


  tft.fillCircle(CX, CY, R_OUTER);
  tft.fore(BG_R, BG_G, BG_B);
  tft.fillCircle(CX, CY, R_INNER);

  tft.fore(9, 0, 0);
  tft.fillCircle(CX, CY, R_CENTER);

  // --- COG window (static) ---
  const uint16_t W = 126;
  const uint16_t H = 76;

  const uint16_t x1 = CX - W / 2;
  const uint16_t y1 = CY - H / 2;
  const uint16_t x2 = CX + W / 2;
  const uint16_t y2 = CY + H / 2;

  // outer frame
  tft.fore(120, 160, 220);
  tft.roundRect(x1, y1, x2, y2, 10);

  // inner (slightly inset for depth)
  tft.fore(60, 100, 180);
  tft.roundRect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, 8);

  // fixed north pointer in off-orange
  tft.fore(255, 170, 70);
  tft.fillTriangle(
    CX, CY - 124,
    CX - 10, CY - 92,
    CX + 10, CY - 92);

  tft.fore(FG_R, FG_G, FG_B);
}



// sin(angle)*128 for 0..359 using 0..90 LUT
static inline int16_t sinDeg128(uint16_t deg) {
  deg %= 360;

  if (deg <= 90) {
    return (int16_t)pgm_read_byte(&sin90_u8_128[deg]);
  }
  if (deg <= 180) {
    return (int16_t)pgm_read_byte(&sin90_u8_128[180 - deg]);
  }
  if (deg <= 270) {
    return -(int16_t)pgm_read_byte(&sin90_u8_128[deg - 180]);
  }
  return -(int16_t)pgm_read_byte(&sin90_u8_128[360 - deg]);
}

static inline int16_t cosDeg128(uint16_t deg) {
  return sinDeg128((uint16_t)(deg + 90));
}

// 0° = straight up, 90° = right
static inline Pos polarXY(int16_t cx, int16_t cy, uint16_t angDeg, uint16_t radius) {
  Pos p;
  p.x = cx + (int16_t)((radius * cosDeg128(angDeg)) / 128);
  p.y = cy - (int16_t)((radius * sinDeg128(angDeg)) / 128);
  return p;
}

static inline uint16_t relAngle(uint16_t bearing, uint16_t heading) {
  int16_t a = (int16_t)heading - (int16_t)bearing + 90;
  if (a < 0) a += 360;
  if (a >= 360) a -= 360;
  return (uint16_t)a;
}


static inline void drawLetterAtPos(Pos p, char c,
                                   uint8_t r, uint8_t g, uint8_t b) {
  const int16_t CW = 20;
  const int16_t CH = 40;

  tft.fore(r, g, b);
  tft.back(BG_R, BG_G, BG_B);  //

  tft.textSize(3);
  tft.textScale(1);

  tft.setCursor((uint16_t)(p.x - CW / 2), (uint16_t)(p.y - CH / 2));

  tft.write(c);
}


static void drawCompassBezelAt(uint16_t deg, uint8_t r, uint8_t g, uint8_t b) {

  const int16_t CX = 520;
  const int16_t CY = 330;

  const uint16_t R_TEXT = 140;      // letters
  const uint16_t R_TICK_OUT = 168;  // outer end of moving ticks
  const uint16_t R_TICK_IN = 148;   // shorter ticks = more "open"

  tft.fore(r, g, b);

  // 30° ticks only, but skip cardinals where letters sit
  for (uint16_t a = 0; a < 360; a += 30) {

    if ((a == 0) || (a == 90) || (a == 180) || (a == 270)) continue;

    uint16_t ra = relAngle(a, deg);

    Pos p1 = polarXY(CX, CY, ra, R_TICK_OUT);
    Pos p2 = polarXY(CX, CY, ra, R_TICK_IN);

    tft.line((uint16_t)p1.x, (uint16_t)p1.y,
             (uint16_t)p2.x, (uint16_t)p2.y);
  }

  // bigger letters
  drawLetterAtPos(polarXY(CX, CY, relAngle(0, deg), R_TEXT), 'N', r, g, b);
  drawLetterAtPos(polarXY(CX, CY, relAngle(90, deg), R_TEXT), 'E', r, g, b);
  drawLetterAtPos(polarXY(CX, CY, relAngle(180, deg), R_TEXT), 'S', r, g, b);
  drawLetterAtPos(polarXY(CX, CY, relAngle(270, deg), R_TEXT), 'W', r, g, b);
}

static void updateCompassBezel(uint16_t deg) {

  static uint16_t lastDeg = 0xFFFF;

  // if (deg >= 360) deg %= 360;
  // if (deg == lastDeg) return;

  // erase old rotating bezel in background color
  if (lastDeg != 0xFFFF) {
    drawCompassBezelAt(lastDeg, BG_R, BG_G, BG_B);
  }

  // draw new rotating bezel in foreground color
  drawCompassBezelAt(deg, FG_R, FG_G, FG_B);

  lastDeg = deg;
}


static inline void showCOG(uint16_t cog) {

  const uint16_t CX = 520;
  const uint16_t CY = 330;

  const uint16_t DX = 32;       // 32
  const uint16_t x0 = CX - 56;  // 48
  const uint16_t y0 = CY - 32;  // 24

  static uint16_t last_cog = 0xFFFF;
  if (cog == last_cog) return;
  last_cog = cog;

  if (cog >= 360) cog %= 360;

  const char c0 = (char)('0' + (cog / 100));
  const char c1 = (char)('0' + ((cog / 10) % 10));
  const char c2 = (char)('0' + (cog % 10));

  tft.textSize(3);
  tft.textScale(2);

  // clear digit area
  tft.fore(0, 0, 0);
  tft.fillRect(x0 - 4, y0 - 4,
               x0 + 3 * DX + 20,
               y0 + 52);

  // draw digits
  tft.fore(230, 230, 230);

  tft.setCursor(x0 + 0 * DX, y0);
  tft.write(c0);

  tft.setCursor(x0 + 1 * DX, y0);
  tft.write(c1);

  tft.setCursor(x0 + 2 * DX, y0);
  tft.write(c2);

  tft.textScale(1);
}


static void drawAxesDemo() {

  const uint16_t ORG_X = 15;
  const uint16_t ORG_Y = 15;

  const uint16_t LEN = 40;

  // --- origin ---
  tft.fore(200, 200, 200);
  tft.fillCircle(ORG_X, ORG_Y, 2);

  // --- X axis (→) ---
  tft.fore(255, 100, 100);
  tft.line(ORG_X, ORG_Y, ORG_X + LEN, ORG_Y);

  // arrow head
  tft.line(ORG_X + LEN, ORG_Y,
           ORG_X + LEN - 6, ORG_Y - 4);
  tft.line(ORG_X + LEN, ORG_Y,
           ORG_X + LEN - 6, ORG_Y + 4);

  // --- Y axis (↓) ---
  tft.fore(100, 220, 140);
  tft.line(ORG_X, ORG_Y, ORG_X, ORG_Y + LEN);

  // arrow head
  tft.line(ORG_X, ORG_Y + LEN,
           ORG_X - 4, ORG_Y + LEN - 6);
  tft.line(ORG_X, ORG_Y + LEN,
           ORG_X + 4, ORG_Y + LEN - 6);

  // =========================
  // "x" label (made of lines)
  // =========================
  const uint16_t XL = ORG_X + LEN + 10;
  const uint16_t YL = ORG_Y + 6;

  tft.fore(255, 100, 100);
  tft.line(XL - 3, YL - 3, XL + 3, YL + 3);
  tft.line(XL - 3, YL + 3, XL + 3, YL - 3);

  // =========================
  // "y" label (made of lines)
  // =========================
  const uint16_t YX = ORG_X + 6;
  const uint16_t YY = ORG_Y + LEN + 10;

  tft.fore(100, 220, 140);

  // arms
  tft.line(YX, YY, YX - 3, YY - 4);
  tft.line(YX, YY, YX + 3, YY - 4);

  // stem
  tft.line(YX, YY, YX, YY + 5);
}




//** Arduino Demo-Program
void setup() {

  SPI.begin();  // start SPI hardware
  tft.begin();  // run TFT init sequence

  // colors
  tft.back(8, 18, 40);
  tft.fore(230, 230, 230);

  drawBackground();
  drawAxesDemo();
  drawPrimitivesDemo();
  draw7SegDemo();
  drawTextDemo();

  // Labels:
  tft.fore(110, 130, 170);
  tft.textSize(2);
  tft.textScale(2);

  tft.back(0, 0, 0);
  tft.setCursor(340, 30);  //
  tft.print("LT7683 / RA8876");
 tft.textSize(2);
  tft.textScale(1);
 tft.setCursor(840, 560);  //
  tft.print("\xA9"
  " by ToSStudio");    // 0xA9

  tft.fore(240, 80, 80);  // soft red
  tft.textSize(3);
  tft.textScale(1);
  tft.setCursor(490, 90);  //
  tft.print("Demo");

  // BMW spinner static rings
  tft.fore(170, 190, 230);
  tft.circle(880, 460, 55);
  tft.fore(220, 230, 245);
  tft.circle(880, 460, 40);
  drawBMW(0);

  // compass
  drawBezelStatic();
  updateCompassBezel(0);
}

void loop() {

  static uint32_t lastBMW = 0;
  static uint8_t bmwState = 0;

  static uint32_t lastCompass = 0;
  static uint16_t demoHeading = 0;

  // slow BMW spinner
  if (millis() - lastBMW >= 900) {
    lastBMW = millis();
    bmwState ^= 1;
    drawBMW(bmwState);
  }

  // smooth compass rotation
  if (millis() - lastCompass >= 120) {
    lastCompass = millis();
    demoHeading++;
    if (demoHeading >= 360) demoHeading = 0;

    updateCompassBezel(demoHeading);
    showCOG(demoHeading);
  }
}
