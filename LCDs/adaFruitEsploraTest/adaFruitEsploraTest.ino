

#include <TFT.h>    // Core graphics library
#include <SPI.h>
#include <Esplora.h>

#define TFT_CS        7
#define TFT_RST        1
#define TFT_DC         0
#define TFT_MOSI 16  // Data out
#define TFT_SCLK 15  // Clock out
#define TFT_BG 0x0F7D
#define TFT_TXT 0x1587
#define TFT_BLK 0x0000
#define L2 25
#define L3 45
#define L4 65
#define L5 85
#define L6 105
#define L7 125

float p = 3.1415926;
int count = 0;

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));
  EsploraTFT.begin();

  Serial.println(F("Initialized"));

  // Screen 160wide x 128 high (0-159 x 0-127)
  // clear the screen with a black background
  EsploraTFT.fillScreen(TFT_BG);
  EsploraTFT.setTextColor(TFT_TXT, TFT_BG);

  EsploraTFT.setTextSize(2);


  Serial.println("done");
  delay(1000);
}

void loop() {
  EsploraTFT.setCursor(28, 5); // Position at top-left corner
  EsploraTFT.setTextColor(TFT_BLK);
  EsploraTFT.print("Thrust"); // Print a message
  EsploraTFT.setTextColor(TFT_TXT, TFT_BG);
  EsploraTFT.setCursor(10, L2); // Position at top-left corner
  EsploraTFT.print(count); // Print a message
  EsploraTFT.print("%"); // Print a message
  EsploraTFT.setCursor(74, L2); // Position at top-left corner
  EsploraTFT.print(count); // Print a message
  EsploraTFT.print("%"); // Print a message



  delay(5000);
  count++;
  Serial.println("done");
}
