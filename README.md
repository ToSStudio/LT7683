# LT7683 / RA8876 Graphics Library

![LT7683 Demo](docs/LT7683_Demo.gif)

Lightweight graphics library for **LT7683 / RA8876 TFT controllers** on Arduino-compatible platforms.

Designed for **performance, clarity, and minimal overhead**, with direct register access and fast primitives.

---

## ✨ Features

* Fast drawing primitives:
* Lines, rectangles, circles, triangles, arcs
* Filled shapes and rounded rectangles
* Text rendering with size and scale control (inherits from print)
* 7-segment display helpers
* RGB color control (foreground / background)
* Optimized for SPI-driven displays
* Minimal dependencies (Arduino + SPI)

---

## 📦 Installation

### Arduino Library Manager (recommended)

1. Open Arduino IDE  
2. Go to **Tools → Manage Libraries…**  
3. Search for: `LT7683`  
4. Click **Install**

### Manual installation

Download this repository as ZIP and install via:

**Sketch → Include Library → Add .ZIP Library…**

---

## ▶️ Quick Start

Open the example:

**File → Examples → LT7683 → LT7683_Demo**

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

## 🚀 Getting Started ("First Light")

```cpp
#include <LT7683.h>

LT7683 tft(CS_PIN, RST_PIN); // use your pins

void setup() {
  SPI.begin();
  tft.begin();
  tft.clearScreen();
  tft.back(0, 0, 0);
  tft.fore(255, 255, 255);

  tft.circle(200, 200, 50);
}

void loop() {
}
```

---

## 🧩 API Overview

### Initialization

```cpp
LT7683 tft(CS_PIN, RST_PIN);
```

* `CS_PIN`  → SPI chip select
* `RST_PIN` → display reset

---

### Print Compatibility

The library inherits from Arduino's `Print` class.

This allows usage of:

* `print()`
* `println()`
* `write()`

Example:

```cpp
tft.setCursor(100, 100);
tft.print("Hello");
```

---

### Drawing Primitives

* `line()`
* `rect()`, `fillRect()`
* `circle()`, `fillCircle()`
* `triangle()`, `fillTriangle()`
* `arc()`, `fillArc()`

---

### Text

### 🔤 Text

Text rendering is controlled via **size** and **scale**:

```
textSize(uint8_t size)
textScale(uint8_t scale)
```

* `textSize`: **1–3** (font selection)
* `textScale`: **1–3** (pixel scaling)

**Example:**

```cpp
tft.textSize(2);
tft.textScale(2);
tft.setCursor(100, 100);
tft.print("Hello");
```

Maximum effective size = textSize × textScale

---

### 🎨 Colors

Colors are specified as 8-bit RGB values:

```
fore(uint8_t r, uint8_t g, uint8_t b)
back(uint8_t r, uint8_t g, uint8_t b)
```

Each component ranges from **0–255**.

**Example:**

```cpp
tft.fore(0, 0, 0);       // black
tft.fore(255, 255, 255); // white
tft.fore(255, 0, 0);     // red
tft.fore(0, 255, 0);     // green
tft.fore(255, 255, 0);   // yellow
tft.fore(0, 120, 255);   // blue-ish

tft.back(0, 0, 0);       // black background
```

> Note: Colors are internally mapped to the active display format (e.g. RGB565).

---

### 7-Segment Rendering

7-segment digits are rendered using primitives and are not part of `Print`.

Example:

```cpp
tft.set7SegScale(2);
tft.plot7Seg(200, 200, 5);
```

Features:

* Fully scalable
* No font required
* Clean rendering at large sizes

---

## 🧠 Controller Notes

The LT7683 (RA8876-compatible) is a hardware graphics controller.

* All drawing operations are executed on the controller itself
* The MCU only sends commands (not pixels)
* This enables smooth animations even on slower MCUs

Coordinate system:

* Origin: top-left (0,0)
* X → right
* Y → down

Compatibility:
* this code is intended for the 7" & 10" TFT Modules with a resolution of 1024 * 600 px as sold by Eastrising / BuyDisplay

---

## ⚡ Interface & Performance

* SPI interface (4-Wire, tested on SAMD21, ATmega32u4, ATtiny)
* Stable operation at typical Arduino SPI speeds
* No framebuffer required on MCU side

---

## 🔢 7-Segment Rendering Details

The 7-segment renderer is built entirely from drawing primitives.

* No bitmap fonts
* Resolution independent
* Suitable for very large digits (tested up to high scaling factors)

---

## 🔌 Wiring 

Typical SPI wiring:

* MOSI → Display SDI  
* MISO ← Display SDO *(required for status reads)*  
* SCK  → Display SCK  
* CS   → Chip Select (CS#)  
* RST  → Reset (RESET#)  

Refer to your specific LT7683 board for exact pinout.

**Important:**

* **MISO (SDO) must be connected**  
  → required for:
  * `check_busy_draw()`  
  * `status_read()`

* The library uses **hardware status polling** instead of fixed delays  
  → ensures smooth and reliable rendering

* If MISO is not connected:
  * drawing may still work  
  * but timing becomes unreliable  
  * animations may stutter


## 🧪 Tested Platforms

* SAMD21 (Feather M0)
* ATmega32u4
* ATtiny (I2C/SPI variants)

---

## 🧱 Structure

```
src/        → library source files
examples/   → demo sketches
docs/       → images / GIFs
```

---

## 📄 License

MIT License

---

## ©

© ToSStudio
