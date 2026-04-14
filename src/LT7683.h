#ifndef LT7683_H
#define LT7683_H

#include <Arduino.h>
#include <SPI.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif
#endif

class LT7683 : public Print {
public:
  using Print::write;

  struct Pos {
    int16_t x;
    int16_t y;
  };

  struct InitCmd {
    uint8_t reg;
    uint8_t val;
  };

  LT7683(uint8_t csPin, uint8_t rstPin);

  virtual size_t write(uint8_t c) override;

  void writeReg(uint8_t reg, uint8_t data);

  void begin();
  void runInitTable(const InitCmd *table, uint16_t count);

  void graphicMode();
  void textMode();
  void displayOn();

  void textSize(uint8_t size);
  void textScale(uint8_t mul);

  void setCursor(uint16_t x, uint16_t y);
  void drawChar(uint16_t x, uint16_t y, char c);
  void textBegin();
  void textEnd();
  void writeChar(char c);
  void printProgmemN(const char *p, uint8_t n);

  void plotDigit(char d);
  void putC(char c);
  void put_u8(uint8_t v);
  void put4(uint16_t v);
  void put2(uint8_t v);

  void setFrameAddress(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

  void line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

  void rect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
  void fillRect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

  void roundRect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint8_t radius);
  void fillRoundRect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint8_t radius);

  void ellipse(uint16_t x, uint16_t y, uint16_t radius1, uint16_t radius2);
  void fillEllipse(uint16_t x, uint16_t y, uint16_t radius1, uint16_t radius2);

  void circle(uint16_t x, uint16_t y, uint16_t radius);
  void fillCircle(uint16_t x, uint16_t y, uint16_t radius);

  void triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
  void fillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);

  void arc(uint16_t x, uint16_t y, uint16_t radius1, uint16_t radius2, uint8_t select);
  void fillArc(uint16_t x, uint16_t y, uint16_t radius1, uint16_t radius2, uint8_t select);

  void plot7Seg(uint16_t x, uint16_t y, uint8_t number);
  void plot7SegRound(uint16_t x, uint16_t y, uint8_t c);
  void drawColon(uint16_t x, uint16_t y, bool on);

  void clearScreen();
  void fore(uint8_t red, uint8_t green, uint8_t blue);
  void back(uint8_t red, uint8_t green, uint8_t blue);

  static uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b);

  void set7SegScale(uint8_t scale);

private:
  uint8_t CS_8876;
  uint8_t RST;

  uint16_t xBase;
  uint16_t yBase;

  uint8_t sc;

  uint8_t rf, gf, bf;
  uint8_t rb, gb, bb;

  static constexpr uint8_t RA8876_SPI_CMDWRITE = 0x00;
  static constexpr uint8_t RA8876_SPI_DATAWRITE = 0x80;
  static constexpr uint8_t RA8876_SPI_DATAREAD = 0xC0;
  static constexpr uint8_t RA8876_SPI_STATUSREAD = 0x40;

  static const uint8_t digits[19];
  static const InitCmd initCode[];

  void GPIO_setup();
  void cmdWrite(uint8_t cmd);
  void dataWrite(uint8_t data);
  uint8_t readReg(uint8_t reg);
  uint8_t RA8876_StatusRead();
  void Check_Busy_Draw();

  void drawH(uint16_t xL, uint16_t xR, uint16_t yT, uint16_t sc);
  void drawV(uint16_t xL, uint16_t yT, uint16_t yB, uint16_t sc);
};

#endif
