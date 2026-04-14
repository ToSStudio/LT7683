#include "LT7683.h"

// ------------------------------------------------------------
// LUTs
// ------------------------------------------------------------

const uint8_t LT7683::digits[19] PROGMEM = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71,
  0x00, 0x40, 0x80
};

const LT7683::InitCmd LT7683::initCode[] PROGMEM = {
  {0x01,0x01},{0xE0,0x29},{0xE1,0x03},{0xE2,0x0B},{0xE3,0x06},{0xE4,0x01},
  {0x02,0x00},{0x03,0x00},
  {0x10,0x04},{0x12,0x80},{0x13,0xB0},
  {0x14,0x7F},{0x15,0x00},{0x16,0x12},{0x17,0xF8},{0x18,0x1A},{0x19,0x07},
  {0x1A,0x57},{0x1B,0x02},{0x1C,0x10},{0x1D,0x80},{0x1E,0x0E},{0x1F,0x04},
  {0x25,0x04},
  {0x50,0x00},{0x51,0x00},{0x52,0x00},{0x53,0x00},{0x54,0x00},{0x55,0x04},
  {0x5B,0x04},{0x5C,0x58},{0x5D,0x02},
  {0x5E,0x01},
  {0x0B,0x01},{0x0C,0x11},{0x0D,0x01},{0x85,0x12},{0x86,0x0B},{0x88,0xFF},{0x89,0xFF},
  {0xCC,0x20},{0xCD,0x80}
};

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------

LT7683::LT7683(uint8_t csPin, uint8_t rstPin)
: CS_8876(csPin), RST(rstPin),
  xBase(0), yBase(0), sc(1),
  rf(255), gf(255), bf(255),
  rb(0), gb(0), bb(0) {}

// ------------------------------------------------------------
// Low-level
// ------------------------------------------------------------

void LT7683::GPIO_setup() {
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);

  pinMode(CS_8876, OUTPUT);
  digitalWrite(CS_8876, HIGH);

  digitalWrite(RST, LOW);
  delay(20);
  digitalWrite(RST, HIGH);
  delay(10);
}

void LT7683::cmdWrite(uint8_t cmd) {
  digitalWrite(CS_8876, LOW);
  SPI.transfer(RA8876_SPI_CMDWRITE);
  SPI.transfer(cmd);
  digitalWrite(CS_8876, HIGH);
}

void LT7683::dataWrite(uint8_t data) {
  digitalWrite(CS_8876, LOW);
  SPI.transfer(RA8876_SPI_DATAWRITE);
  SPI.transfer(data);
  digitalWrite(CS_8876, HIGH);
}

uint8_t LT7683::readReg(uint8_t reg) {
  digitalWrite(CS_8876, LOW);
  SPI.transfer(RA8876_SPI_CMDWRITE);
  SPI.transfer(reg);
  SPI.transfer(RA8876_SPI_DATAREAD);
  uint8_t d = SPI.transfer(0xFF);
  digitalWrite(CS_8876, HIGH);
  return d;
}

uint8_t LT7683::RA8876_StatusRead() {
  digitalWrite(CS_8876, LOW);
  SPI.transfer(RA8876_SPI_STATUSREAD);
  uint8_t d = SPI.transfer(0xFF);
  digitalWrite(CS_8876, HIGH);
  return d;
}

void LT7683::Check_Busy_Draw() {
  while (RA8876_StatusRead() & 0x08) {
    delayMicroseconds(20);
  }
}

void LT7683::writeReg(uint8_t reg, uint8_t data) {
  digitalWrite(CS_8876, LOW);
  SPI.transfer(RA8876_SPI_CMDWRITE);
  SPI.transfer(reg);
  SPI.transfer(RA8876_SPI_DATAWRITE);
  SPI.transfer(data);
  digitalWrite(CS_8876, HIGH);
}

// ------------------------------------------------------------
// Init
// ------------------------------------------------------------

void LT7683::runInitTable(const InitCmd *table, uint16_t count) {
  for (uint16_t i = 0; i < count; i++) {
    writeReg(pgm_read_byte(&table[i].reg),
             pgm_read_byte(&table[i].val));
  }
}

void LT7683::begin() {
  GPIO_setup();
  runInitTable(initCode, sizeof(initCode)/sizeof(initCode[0]));
  graphicMode();
  displayOn();
}

// ------------------------------------------------------------
// Modes
// ------------------------------------------------------------

void LT7683::graphicMode() {
  writeReg(0x03, readReg(0x03) & ~0x04);
}

void LT7683::displayOn() {
  writeReg(0x12, readReg(0x12) | 0x40);
}

// ------------------------------------------------------------
// Text control
// ------------------------------------------------------------

void LT7683::textMode() {
  writeReg(0x03, readReg(0x03) | 0x04);
}

void LT7683::textSize(uint8_t size) {
  // valid LT7683 internal font sizes are typically 1..4
  if (size < 1) size = 1;
  if (size > 4) size = 4;

  writeReg(0xCC, (uint8_t)((size - 1) << 4));
}

// CCR1 (0xCD):
// [7]   = full alignment
// [6]   = text chroma-key background
// [4]   = rotation
// [3:2] = width scale  (0..3 => x1..x4)
// [1:0] = height scale (0..3 => x1..x4)
void LT7683::textScale(uint8_t mul) {
  if (mul < 1) mul = 1;
  if (mul > 4) mul = 4;

  uint8_t temp = readReg(0xCD);

  uint8_t v  = (uint8_t)(mul - 1);       // 0..3
  uint8_t wh = (uint8_t)((v << 2) | v);  // same factor for width and height

  temp = (uint8_t)((temp & 0xF0) | wh);  // keep upper control bits
  writeReg(0xCD, temp);
}


// ------------------------------------------------------------
// Text
// ------------------------------------------------------------

void LT7683::setCursor(uint16_t x, uint16_t y) {
  if (x > 1023) x = 1023;
  if (y > 599) y = 599;

  xBase = x;
  yBase = y;

  writeReg(0x63, x);
  writeReg(0x64, x >> 8);
  writeReg(0x65, y);
  writeReg(0x66, y >> 8);
}

size_t LT7683::write(uint8_t c) {

  if (c == '\r') {
    setCursor(0, yBase);
    return 1;
  }

  if (c == '\n') {
    setCursor(0, yBase + 32 * sc);
    return 1;
  }

  textMode();
  cmdWrite(0x04);
  dataWrite(c);
  Check_Busy_Draw();
  graphicMode();

  return 1;
}

// ------------------------------------------------------------
// Primitives
// ------------------------------------------------------------

void LT7683::setFrameAddress(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
  if (x0>1023)x0=1023;if(y0>599)y0=599;if(x1>1023)x1=1023;if(y1>599)y1=599;
  writeReg(0x68,x0);writeReg(0x69,x0>>8);
  writeReg(0x6A,y0);writeReg(0x6B,y0>>8);
  writeReg(0x6C,x1);writeReg(0x6D,x1>>8);
  writeReg(0x6E,y1);writeReg(0x6F,y1>>8);
}

void LT7683::line(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
  setFrameAddress(x0,y0,x1,y1);
  writeReg(0x67,0x80);
  Check_Busy_Draw();
}

void LT7683::rect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
  setFrameAddress(x0,y0,x1,y1);
  writeReg(0x76,0xA0);
  Check_Busy_Draw();
}

void LT7683::fillRect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
  setFrameAddress(x0,y0,x1,y1);
  writeReg(0x76,0xE0);
  Check_Busy_Draw();
}

void LT7683::roundRect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint8_t r){
  setFrameAddress(x0,y0,x1,y1);
  writeReg(0x77,r);writeReg(0x79,r);
  writeReg(0x76,0xB0);
  Check_Busy_Draw();
}

void LT7683::fillRoundRect(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint8_t r){
  setFrameAddress(x0,y0,x1,y1);
  writeReg(0x77,r);writeReg(0x79,r);
  writeReg(0x76,0xF0);
  Check_Busy_Draw();
}

void LT7683::ellipse(uint16_t x,uint16_t y,uint16_t r1,uint16_t r2){
  writeReg(0x7B,x);writeReg(0x7C,x>>8);
  writeReg(0x7D,y);writeReg(0x7E,y>>8);
  writeReg(0x77,r1);writeReg(0x78,r1>>8);
  writeReg(0x79,r2);writeReg(0x7A,r2>>8);
  writeReg(0x76,0x80);
  Check_Busy_Draw();
}

void LT7683::fillEllipse(uint16_t x,uint16_t y,uint16_t r1,uint16_t r2){
  ellipse(x,y,r1,r2);
  writeReg(0x76,0xC0);
  Check_Busy_Draw();
}

void LT7683::circle(uint16_t x,uint16_t y,uint16_t r){
  ellipse(x,y,r,r);
}

void LT7683::fillCircle(uint16_t x,uint16_t y,uint16_t r){
  fillEllipse(x,y,r,r);
}

void LT7683::triangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3){
 
 // point 1
  writeReg(0x68, x1);
  writeReg(0x69, x1 >> 8);
  writeReg(0x6A, y1);
  writeReg(0x6B, y1 >> 8);

  // point 2
  writeReg(0x6C, x2);
  writeReg(0x6D, x2 >> 8);
  writeReg(0x6E, y2);
  writeReg(0x6F, y2 >> 8);

  // point 3
  writeReg(0x70, x3);
  writeReg(0x71, x3 >> 8);
  writeReg(0x72, y3);
  writeReg(0x73, y3 >> 8);

  // start draw, non-filled: bit7=1, bit5=0
  writeReg(0x67, 0x82);
  Check_Busy_Draw();

}

void LT7683::fillTriangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3){
  // point 1
  writeReg(0x68, x1);
  writeReg(0x69, x1 >> 8);
  writeReg(0x6A, y1);
  writeReg(0x6B, y1 >> 8);

  // point 2
  writeReg(0x6C, x2);
  writeReg(0x6D, x2 >> 8);
  writeReg(0x6E, y2);
  writeReg(0x6F, y2 >> 8);

  // point 3
  writeReg(0x70, x3);
  writeReg(0x71, x3 >> 8);
  writeReg(0x72, y3);
  writeReg(0x73, y3 >> 8);

  writeReg(0x67, 0xA2);  // filled triangle
  Check_Busy_Draw();
}

void LT7683::arc(uint16_t x,uint16_t y,uint16_t r1,uint16_t r2,uint8_t s){
  writeReg(0x7B,x);writeReg(0x7C,x>>8);
  writeReg(0x7D,y);writeReg(0x7E,y>>8);
  writeReg(0x77,r2);writeReg(0x78,r2>>8);
  writeReg(0x79,r1);writeReg(0x7A,r1>>8);
  writeReg(0x76,0x90|s);
  Check_Busy_Draw();
}

void LT7683::fillArc(uint16_t x,uint16_t y,uint16_t r1,uint16_t r2,uint8_t s){
  arc(x,y,r1,r2,s);
  writeReg(0x76,0xD0|s);
  Check_Busy_Draw();
}

// ------------------------------------------------------------
// Colors / misc
// ------------------------------------------------------------

void LT7683::clearScreen(){
  uint8_t r=rf,g=gf,b=bf;
  fore(0,0,0);
  fillRect(0,0,1023,599);
  fore(r,g,b);
}

void LT7683::fore(uint8_t r,uint8_t g,uint8_t b){
  rf=r;gf=g;bf=b;
  writeReg(0xD2,r);writeReg(0xD3,g);writeReg(0xD4,b);
}

void LT7683::back(uint8_t r,uint8_t g,uint8_t b){
  rb=r;gb=g;bb=b;
  writeReg(0xD5,r);writeReg(0xD6,g);writeReg(0xD7,b);
}

uint16_t LT7683::RGB565(uint8_t r,uint8_t g,uint8_t b){
  return ((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3);
}

// ------------------------------------------------------------
// 7-seg
// ------------------------------------------------------------

void LT7683::set7SegScale(uint8_t s){
  if(s<1)s=1;if(s>10)s=10;
  sc=s;
}

void LT7683::drawH(uint16_t xL, uint16_t xR, uint16_t yT, uint16_t sc) {
  uint16_t yB = yT + 2 * sc;
  fillTriangle(xL - sc, yT + sc, xL, yT, xL, yB);
  fillRect(xL, yT, xR, yB);
  fillTriangle(xR, yT, xR + sc, yT + sc, xR, yB);
}

void LT7683::drawV(uint16_t xL, uint16_t yT, uint16_t yB, uint16_t sc) {
  uint16_t xR = xL + 2 * sc;
  fillTriangle(xL, yT, xL + sc, yT - sc, xR, yT);
  fillRect(xL, yT, xR, yB);
  fillTriangle(xL, yB, xL + sc, yB + sc, xR, yB);
}

void LT7683::plot7Seg(uint16_t x, uint16_t y, uint8_t number) {

  if (number >= 19) number = 16;  // fallback = space

  uint8_t seg = pgm_read_byte(&digits[number]);

  // save current colors locally
  uint8_t fr = rf, fg = gf, fb = bf;
  uint8_t br = rb, bg = gb, bb2 = bb;

  uint16_t x0  = x;
  uint16_t x2  = x + 2 * sc;
  uint16_t x10 = x + 10 * sc;
  uint16_t x12 = x + 12 * sc;

  uint16_t y0  = y;
  uint16_t y2  = y + 2 * sc;
  uint16_t y11 = y + 11 * sc;
  uint16_t y13 = y + 13 * sc;
  uint16_t y22 = y + 22 * sc;
  uint16_t y24 = y + 24 * sc;

  // Segment 1
  if (seg & 0x01) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawH(x2, x10, y0, sc);

  // Segment 2
  if (seg & 0x02) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawV(x10, y2, y11, sc);

  // Segment 3
  if (seg & 0x04) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawV(x10, y13, y22, sc);

  // Segment 4
  if (seg & 0x08) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawH(x2, x10, y22, sc);

  // Segment 5
  if (seg & 0x10) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawV(x0, y13, y22, sc);

  // Segment 6
  if (seg & 0x20) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawV(x0, y2, y11, sc);

  // Segment 7
  if (seg & 0x40) fore(fr, fg, fb);
  else            fore(br, bg, bb2);
  drawH(x2, x10, y11, sc);

  // Segment 8: decimal point
  if (seg & 0x80) {
    fore(fr, fg, fb);
    uint16_t dx = x12 - sc;
    uint16_t dy = y24 - sc;
    uint8_t dr = (sc > 1) ? (sc - 1) : 1;
    fillCircle(dx, dy, dr);
  }

  // restore original foreground
  fore(fr, fg, fb);
}


void LT7683::plot7SegRound(uint16_t x, uint16_t y, uint8_t c) {

  switch (c) {

    case 0:
      fillRect  (x + (2 * sc), y, x + (10 * sc), y + (2 * sc));               // top horizontal
      fillArc   (x + (10 * sc), y + (2 * sc), 2 * sc, 2 * sc, 2);             // top right arc
      fillRect  (x + (10 * sc), y + (2 * sc), x + (12 * sc), y + (22 * sc));  // right vertical
      fillArc   (x + (10 * sc), y + (22 * sc), 2 * sc, 2 * sc, 3);            // bottom right arc
      fillRect  (x + (2 * sc), y + (22 * sc), x + (10 * sc), y + (24 * sc));  // bottom horizontal
      fillArc   (x + (2 * sc), y + (22 * sc), 2 * sc, 2 * sc, 0);             // bottom left arc
      fillRect  (x, y + (2 * sc), x + (2 * sc), y + (22 * sc));               // left vertical
      fillArc   (x + (2 * sc), y + (2 * sc), 2 * sc, 2 * sc, 1);              // top left arc
      break;

    case 1:
      fillCircle(x + (11 * sc), y + sc, sc);                                  // top right tip
      fillRect  (x + (10 * sc), y + sc, x + (12 * sc), y + (23 * sc));        // right vertical
      fillCircle(x + (11 * sc), y + (23 * sc), sc);                           // bottom right tip
      break;

    case 2:
      fillRect  (x + sc, y, x + (10 * sc), y + (2 * sc));                     // top horizontal
      fillCircle(x + sc, y + sc, sc);                                         // top left tip
      fillArc   (x + (10 * sc), y + (2 * sc), 2 * sc, 2 * sc, 2);            // top right arc
      fillRect  (x + (10 * sc), y + (2 * sc), x + (12 * sc), y + (11 * sc)); // top right vertical
      fillArc   (x + (10 * sc), y + (11 * sc), 2 * sc, 2 * sc, 3);           // center right arc
      fillRect  (x + (2 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      fillArc   (x + (2 * sc), y + (13 * sc), 2 * sc, 2 * sc, 1);            // center left arc
      fillRect  (x, y + (13 * sc), x + (2 * sc), y + (22 * sc));             // left bottom vertical
      fillArc   (x + (2 * sc), y + (22 * sc), 2 * sc, 2 * sc, 0);            // bottom left arc
      fillRect  (x + (2 * sc), y + (22 * sc), x + (11 * sc), y + (24 * sc)); // bottom horizontal
      fillCircle(x + (11 * sc), y + (23 * sc), sc);                          // bottom right tip
      break;

    case 3:
      fillCircle(x + sc, y + sc, sc);                                         // top left tip
      fillRect  (x + sc, y, x + (10 * sc), y + (2 * sc));                     // top horizontal
      fillArc   (x + (10 * sc), y + (2 * sc), 2 * sc, 2 * sc, 2);            // top right arc
      fillRect  (x + (10 * sc), y + (2 * sc), x + (12 * sc), y + (22 * sc)); // right vertical
      fillRect  (x + (3 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      fillCircle(x + (3 * sc), y + (12 * sc), sc);                           // center left tip
      fillArc   (x + (10 * sc), y + (22 * sc), 2 * sc, 2 * sc, 3);           // bottom right arc
      fillRect  (x + sc, y + (22 * sc), x + (10 * sc), y + (24 * sc));       // bottom horizontal
      fillCircle(x + sc, y + (23 * sc), sc);                                 // bottom left tip
      break;

    case 4:
      fillRect  (x + (10 * sc), y + (4 * sc), x + (12 * sc), y + (12 * sc)); // right upper vertical
      fillCircle(x + (11 * sc), y + (4 * sc), sc);                            // top right tip
      fillRect  (x + (10 * sc), y + (12 * sc), x + (12 * sc), y + (23 * sc));// right lower vertical
      fillCircle(x + (11 * sc), y + (23 * sc), sc);                           // bottom right tip
      fillRect  (x + (2 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      fillRect  (x, y + sc, x + (2 * sc), y + (11 * sc));                     // left upper vertical
      fillArc   (x + (2 * sc), y + (11 * sc), 2 * sc, 2 * sc, 0);            // left center arc
      fillCircle(x + sc, y + sc, sc);                                         // top left tip
      break;

    case 5:
      fillRect  (x + (2 * sc), y, x + (11 * sc), y + (2 * sc));               // top horizontal
      fillCircle(x + (11 * sc), y + sc, sc);                                  // top right tip
      fillArc   (x + (2 * sc), y + (2 * sc), 2 * sc, 2 * sc, 1);             // top left arc
      fillRect  (x, y + (2 * sc), x + (2 * sc), y + (11 * sc));              // upper left vertical
      fillArc   (x + (2 * sc), y + (11 * sc), 2 * sc, 2 * sc, 0);            // center left arc
      fillRect  (x + (2 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      fillArc   (x + (10 * sc), y + (13 * sc), 2 * sc, 2 * sc, 2);           // center right arc
      fillArc   (x + (10 * sc), y + (22 * sc), 2 * sc, 2 * sc, 3);           // bottom right arc
      fillRect  (x + (10 * sc), y + (13 * sc), x + (12 * sc), y + (22 * sc));// lower right vertical
      fillRect  (x + (2 * sc), y + (22 * sc), x + (10 * sc), y + (24 * sc)); // bottom horizontal
      fillCircle(x + (2 * sc), y + (23 * sc), sc);                            // bottom left tip
      break;

    case 6:
      fillRect  (x + (2 * sc), y, x + (8 * sc), y + (2 * sc));                // top horizontal
      fillCircle(x + (8 * sc), y + sc, sc);                                   // top right tip
      fillArc   (x + (2 * sc), y + (2 * sc), 2 * sc, 2 * sc, 1);             // top left arc
      fillRect  (x, y + (2 * sc), x + (2 * sc), y + (22 * sc));              // left vertical
      fillArc   (x + (2 * sc), y + (11 * sc), 2 * sc, 2 * sc, 0);            // center left arc
      fillRect  (x + (2 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      fillArc   (x + (10 * sc), y + (13 * sc), 2 * sc, 2 * sc, 2);           // center right arc
      fillArc   (x + (10 * sc), y + (22 * sc), 2 * sc, 2 * sc, 3);           // bottom right arc
      fillRect  (x + (10 * sc), y + (13 * sc), x + (12 * sc), y + (22 * sc));// lower right vertical
      fillRect  (x + (2 * sc), y + (22 * sc), x + (10 * sc), y + (24 * sc)); // bottom horizontal
      fillArc   (x + (2 * sc), y + (22 * sc), 2 * sc, 2 * sc, 0);            // bottom left arc
      break;

    case 7:
      fillCircle(x + (2 * sc), y + sc, sc);                                   // top left tip
      fillRect  (x + (2 * sc), y, x + (10 * sc), y + (2 * sc));               // top horizontal
      fillArc   (x + (10 * sc), y + (2 * sc), 2 * sc, 2 * sc, 2);            // top right arc
      fillRect  (x + (10 * sc), y + (2 * sc), x + (12 * sc), y + (23 * sc)); // right vertical
      fillCircle(x + (11 * sc), y + (23 * sc), sc);                           // bottom tip
      break;

    case 8:
      fillRect  (x + (2 * sc), y, x + (10 * sc), y + (2 * sc));               // top horizontal
      fillArc   (x + (10 * sc), y + (2 * sc), 2 * sc, 2 * sc, 2);            // top right arc
      fillRect  (x + (10 * sc), y + (2 * sc), x + (12 * sc), y + (22 * sc)); // right vertical
      fillArc   (x + (10 * sc), y + (22 * sc), 2 * sc, 2 * sc, 3);           // bottom right arc
      fillRect  (x + (2 * sc), y + (22 * sc), x + (10 * sc), y + (24 * sc)); // bottom horizontal
      fillArc   (x + (2 * sc), y + (22 * sc), 2 * sc, 2 * sc, 0);            // bottom left arc
      fillRect  (x, y + (2 * sc), x + (2 * sc), y + (22 * sc));              // left vertical
      fillArc   (x + (2 * sc), y + (2 * sc), 2 * sc, 2 * sc, 1);             // top left arc
      fillRect  (x + (2 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      break;

    case 9:
      fillRect  (x + (2 * sc), y, x + (10 * sc), y + (2 * sc));               // top horizontal
      fillArc   (x + (10 * sc), y + (2 * sc), 2 * sc, 2 * sc, 2);            // top right arc
      fillRect  (x + (10 * sc), y + (2 * sc), x + (12 * sc), y + (22 * sc)); // right vertical
      fillArc   (x + (10 * sc), y + (22 * sc), 2 * sc, 2 * sc, 3);           // bottom right arc
      fillRect  (x + (4 * sc), y + (22 * sc), x + (10 * sc), y + (24 * sc)); // bottom horizontal
      fillCircle(x + (4 * sc), y + (23 * sc), sc);                            // bottom left tip
      fillArc   (x + (2 * sc), y + (11 * sc), 2 * sc, 2 * sc, 0);            // center left arc
      fillRect  (x, y + (2 * sc), x + (2 * sc), y + (11 * sc));              // left vertical
      fillArc   (x + (2 * sc), y + (2 * sc), 2 * sc, 2 * sc, 1);             // top left arc
      fillRect  (x + (2 * sc), y + (11 * sc), x + (10 * sc), y + (13 * sc)); // center horizontal
      break;
  }
}

void LT7683::drawColon(uint16_t x, uint16_t y, bool on) {

  uint8_t fr = rf, fg = gf, fb = bf;
  uint8_t br = rb, bg = gb, bb2 = bb;

  uint16_t cx  = x + 6 * sc;
  uint16_t cy1 = y + 8 * sc;
  uint16_t cy2 = y + 16 * sc;

  if (on) fore(fr, fg, fb);
  else    fore(br, bg, bb2);

  uint8_t r = 2 * sc;
  fillCircle(cx, cy1, r);
  fillCircle(cx, cy2, r);

  fore(fr, fg, fb);
}

