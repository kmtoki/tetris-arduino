// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <MsTimer2.h>

#include <Tetris.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

const unsigned int COLORS[] = {
  BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE
};

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

const int MY = tft.height() / 2;
const int MX = tft.width() / 2;

const byte FIELD_SIZE_Y = 20;
const byte FIELD_SIZE_X = 10;
const byte RECT_SIZE = 15;

Tetris t = Tetris(FIELD_SIZE_Y, FIELD_SIZE_X);
int dbuffer[FIELD_SIZE_Y][FIELD_SIZE_X];

void setup() {
  randomSeed(analogRead(A3));

  tft.reset();
  tft.begin();
  tft.fillScreen(BLACK);
  pinMode(13, OUTPUT);

  tft.setTextColor(WHITE);
  tft.setTextSize(1);

  tft.drawLine(14, 0, 14, 320, WHITE);
  tft.drawLine(177, 0, 177, 320, WHITE);
  tft.drawLine(177, MY, 320, MY, WHITE);

  t.block_random();
  for (int y = 0; y < FIELD_SIZE_Y; y++) {
    for (int x = 0; x < FIELD_SIZE_X; x++) {
      dbuffer[y][x] = t.field[y][x];
    }
  }

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      int yy = y + t.pos_y;
      int xx = x + t.pos_x;
      if (0 <= yy && yy < t.size_y && 0 <= xx && xx < t.size_x && t.block[y][x] != 0) {
        dbuffer[y][x] = t.block[y][x];
      }
    }
  }
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

int counter = 0;

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
   
    int h = tft.height() / 3;
    int w = tft.width() / 3;

    if (p.y >= MY && p.x >= w && p.x < w * 2) {
      t.block_rotate();
    }
    else if (p.y >= h && p.y < h * 2 && p.x < MX) {
      t.block_left();
    }
    else if (p.y >= h && p.y < h * 2 && p.x >= MX) {
      t.block_right();
    }
    else if (p.y < MY && p.x >= w && p.x < w * 2) {
      t.block_down();
    }

    tetris_draw();
    t.field_remove_fill_line();
  }

  if (counter > 20) {
    t.block_down();
    tetris_draw();
    t.field_remove_fill_line();
    counter = 0;
  }

  counter += 1;
  delay(50);
}

void tetris_draw() {
  tetris_field_draw();
  tetris_block_draw();
  tetris_next_draw();
  tetris_score_draw();
}

void tetris_field_draw() {
  for (int y = 0; y < FIELD_SIZE_Y; y++) {
    for (int x = 0; x < FIELD_SIZE_X; x++) {
      if (dbuffer[y][x] != t.field[y][x]) {
        tft.fillRect(
          (1 + x) * (RECT_SIZE + 1), 
          y * (RECT_SIZE + 1), 
          RECT_SIZE, 
          RECT_SIZE, 
          COLORS[t.field[y][x]]
        );
      }

      dbuffer[y][x] = t.field[y][x];
    }
  }
}

void tetris_block_draw() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      int yy = y + t.pos_y;
      int xx = x + t.pos_x;
      if (0 <= yy && yy < t.size_y && 0 <= xx && xx < t.size_x && t.block[y][x] != 0) {
        if (dbuffer[yy][xx] != t.block[y][x]) {
          tft.fillRect(
            (1 + xx) * (RECT_SIZE + 1), 
            yy * (RECT_SIZE + 1), 
            RECT_SIZE, 
            RECT_SIZE, 
            COLORS[t.block[y][x]]
          );
        }

        dbuffer[yy][xx] = t.block[y][x];
      }
    }
  }
}


void tetris_next_draw() {
  tft.setCursor(180, 20);
  tft.print("next:");
 
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      int c = 0;
      if (BLOCKS[t.next][y][x] != 0) {
        c = t.next_color;
      }
      else {
        c = 0;
      }

      tft.fillRect(
        190 + x * 10,
        50 + y * 10, 
        9, 
        9, 
        COLORS[c]
      );
    }
  }
}

void tetris_score_draw() {
  tft.setCursor(180, 180);
  tft.println("score: ");
  tft.setCursor(190, 200);
  tft.fillRect(190, 200, 20, 20, BLACK);
  tft.print(t.score);
}
