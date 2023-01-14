

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS         5
#define TFT_RST        26
#define TFT_DC         27
#define TFT_MOSI 19  // Data out
#define TFT_SCLK 18  // Clock out
#define TFT_BG 0x0F7D
#define TFT_TXT 0x1587
#define L2 25
#define L3 45
#define L4 65
#define L5 85
#define L6 105
#define L7 125


// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float p = 3.1415926;
int count = 0;

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(4);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(TFT_BG);

  //                Text    Background
  tft.setTextColor(TFT_TXT, TFT_BG);
  tft.setTextWrap(false);

  tft.drawFastHLine(0, 42, 128, ST77XX_BLACK);
  tft.drawFastHLine(0, 82, 128, ST77XX_BLACK);
  tft.cp437(false);

  delay(1000);


  Serial.println("done");
  delay(1000);
}

void loop() {
  tft.setTextSize(2);
  tft.setCursor(28, 5); // Position at top-left corner
  tft.setTextColor(ST77XX_BLACK);
  tft.print("Thrust"); // Print a message
  tft.setTextColor(TFT_TXT, TFT_BG);
  tft.setCursor(10, L2); // Position at top-left corner
  tft.print(count); // Print a message
  tft.print("%"); // Print a message
  tft.setCursor(74, L2); // Position at top-left corner
  tft.print(count); // Print a message
  tft.print("%"); // Print a message

  tft.setCursor(10, L3); // Position at top-left corner
  tft.setTextColor(ST77XX_BLACK);
  tft.print("Bat:"); // Print a message
  tft.setTextColor(TFT_TXT, TFT_BG);
  tft.print(count); // Print a message
  tft.print("V "); // Print a message
  tft.setCursor(10, L4); // Position at top-left corner
  tft.print(count); // Print a message
  tft.print("A "); // Print a message
  tft.print(count); // Print a message
  tft.print("W"); // Print a message

  tft.setCursor(46, L5); // Position at top-left corner
  tft.setTextColor(ST77XX_BLACK);
  tft.print("GPS"); // Print a message
  tft.setTextColor(TFT_TXT, TFT_BG);
  tft.setCursor(10, L6); // Position at top-left corner
  tft.print(count); // Print a message
  tft.print("kmph"); // Print a message
  tft.setCursor(10, L7); // Position at top-left corner
  tft.print(count); // Print a message
  tft.print("Deg"); // Print a message



  delay(5000);
  count++;
}
