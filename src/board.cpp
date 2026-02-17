#include "Particle.h"
#include "Adafruit_ST7735.h"

#define TFT_CS   D5
#define TFT_DC   D6
#define TFT_RST  D7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); //HardwareSPI

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void setup() {
    delay(1000);

    SPI.begin();

    tft.initR(); //changed from begin
    tft.setRotation(1); // May need to be changed
    tft.fillScreen(ST7735_BLACK);

    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.setCursor(40, 40);
    tft.println("Photon + ST7735");
    tft.println("Display OK");

    delay(1000);

    tft.fillScreen(ST7735_RED);
    delay(300);
    tft.fillScreen(ST7735_GREEN);
    delay(300);
    tft.fillScreen(ST7735_BLUE);
    delay(300);
    tft.fillScreen(ST7735_BLACK);
}
void loop() {
}
