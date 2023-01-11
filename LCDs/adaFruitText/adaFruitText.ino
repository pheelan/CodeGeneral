

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS         14
#define TFT_RST        15
#define TFT_DC         32
#define TFT_MOSI 19  // Data out
#define TFT_SCLK 18  // Clock out

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
  tft.setRotation(2);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(false);


  delay(1000);


  Serial.println("done");
  delay(1000);
}

void loop() {

    tft.setCursor(0, 0); // Position at top-left corner
    tft.print("Hello"); // Print a message
    delay(1000); // Pause 1 second
    tft.setCursor(0, 0); // Back to top-left corner
    tft.print("World"); // Print another message, same length
    tft.setCursor(0, 20); // Position at top-left corner
    tft.print(count);

    /*
      tft.setCursor(0, 5);
      tft.setTextColor(ST77XX_BLACK);
      tft.setTextSize(2);
      tft.println("Speed: ");
      tft.setCursor(70, 5);

      tft.print(count);
      tft.print(1234.567);

      tft.setCursor(0, 20);
      tft.print(p, 6);

      tft.setCursor(0, 35);
      tft.print(millis() / 1000);
      tft.setTextColor(ST77XX_WHITE);
      tft.print(" seconds.");
      tft.setCursor(0, 50);
    */

    delay(5000);
    count++;
  }
