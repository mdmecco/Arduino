
#include <SPI.h>
// adatto per wemos

// VCC --> 3.3
// GND --> GND

// CLK --> D5 SCK
// SDA --> D7 MOSI
// RS  --> D1 GPIO5
// RST --> D2 GPIO4
// CS  --> D4 GPIO2

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <EEPROM.h>

#include "DHT.h"
#define DHTPIN 0
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define TFT_CS         2
#define TFT_RST        4 
#define TFT_DC         5

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

int tmax;
int tmin = 50;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  dht.begin();
}

void loop(void){
   int h = dht.readHumidity();
   int t = dht.readTemperature();
  
  tft.setCursor(40,5);
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.println("Temperatura");
  tft.setCursor(40,20);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(3);
  tft.println(t);

  tft.setCursor(40,60);
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.println("Umidita'");
  tft.setCursor(50,75);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(3);
  tft.println(h);

  tft.fillRect(3,105, 122,5, BLUE);
  tft.fillRect(3,110, 122,5, RED);
  tft.fillRect(3,115, 122,5, GREEN);
  tft.fillRect(0,0, 3,160, YELLOW);
  tft.fillRect(0,0, 160,3, YELLOW);
  
  
if (t > tmax) {
  tmax = t;
  EEPROM.write(0, t);
  }

if (t < tmin) {
  tmin = t;
  EEPROM.write(2, t);
  }
  
  tft.setCursor(5,125);
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.println("T.Max");
  tft.setCursor(10,137);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(3);
  tft.println(tmax);

  tft.setCursor(90,125);
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.println("T.Min");
  tft.setCursor(90,137);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(3);
  tft.println(tmin);
  
}
