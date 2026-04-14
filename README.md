# LT7683 / RA8876 Graphics Library

![LT7683 Demo](docs/LT7683_Demo.gif)

Lightweight graphics library for **LT7683 / RA8876 TFT controllers** on Arduino-compatible platforms.

Designed for **performance, clarity, and minimal overhead**, with direct register access and fast primitives.

---

## ✨ Features

* Fast drawing primitives:

  * Lines, rectangles, circles, triangles, arcs
* Filled shapes and rounded rectangles
* Text rendering with size and scale control
* 7-segment display helpers
* RGB color control (foreground / background)
* Optimized for SPI-driven displays
* Minimal dependencies (Arduino + SPI)

---

## 🧪 Demo

The included demo showcases:

* Smooth rotating compass bezel
* Digital COG (course over ground)
* Primitive drawing examples
* Text rendering (size + scaling)
* 7-segment display
* Simple UI elements

Find it here:

examples/LT7683_Demo/LT7683_Demo.ino

---

## 🚀 Getting Started

#include <LT7683.h>

LT7683 tft(CS_PIN, RST_PIN);

void setup() {
SPI.begin();
tft.begin();

tft.back(0, 0, 0);
tft.fore(255, 255, 255);

tft.circle(200, 200, 50);
}

void loop() {
}

---

## 🧱 Structure

src/        → library source files
examples/   → demo sketches
docs/       → images / GIFs

---

## ⚙️ Hardware

Tested with:

* LT7683-based TFT displays (7" / 10")
* RA8876-compatible controllers
* SAMD21 (Feather M0)
* ATtiny (I2C variant in earlier projects)

---

## 📌 Notes

* Library focuses on **low-level control + speed**
* No heavy abstractions — intended for embedded use
* Designed to scale from **tiny MCUs → SAMD-class boards**

---

---

## 🧠 Controller Notes

The LT7683 (RA8876-compatible) is a hardware graphics controller.

- All drawing operations are executed **on the controller itself**
- The MCU only sends commands (not pixels)
- This enables smooth animations even on slower MCUs

Coordinate system:

- Origin: **top-left (0,0)**
- X → right
- Y → down

---

## ⚡ Interface & Performance

- SPI interface (tested on SAMD21, ATmega32u4, ATtiny)
- Stable operation at typical Arduino SPI speeds
- No framebuffer required on MCU side

---

## 🔢 7-Segment Rendering

The library includes a lightweight 7-segment renderer:

- Built **entirely from primitives**
- No bitmap fonts required
- Fully scalable

Typical usage:

- Clean rendering from small sizes up to large displays
- Tested up to high scaling factors (e.g. `scale = 20`)

---

## 🔌 Wiring (basic)

Typical SPI wiring:

- MOSI → Display MOSI
- SCK  → Display SCK
- CS   → Chip Select
- RST  → Reset

(Refer to your specific LT7683 board for exact pinout)

---

## 🧩 API Overview

Core functionality:

- Drawing:
  - `line()`, `rect()`, `circle()`, `triangle()`, `arc()`
- Filled shapes:
  - `fillRect()`, `fillCircle()`, `fillTriangle()`
- Text:
  - `textSize()`, `textScale()`, `setCursor()`, `write()`
- Colors:
  - `fore(r,g,b)`, `back(r,g,b)`

The API is intentionally minimal and close to the hardware.

---

## 🧪 Tested Platforms

- SAMD21 (Feather M0)
- ATmega32u4
- ATtiny (I2C/SPI variants)

---


## 📄 License

MIT License

---

## ©

© ToSStudio

