#include <EEPROM.h>
#include <stdint.h>
#include <TouchScreen.h>
#include <SPI.h>

//#include "A:\Sketch\#Lib\Ethernet-2.0.1\src\Ethernet.h"


#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
MCUFRIEND_kbv tft;
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSerif12pt7b.h"
#include "FreeDefaultFonts.h"
#define PI 3.1415926535897932384626433832795
int col[8];



/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#define PRGVER "28/09/2022"

byte NetMas =0;
String S1;
String S2;
String S3;
bool rp=false;
bool OTAActive=false;
unsigned long NetTo = 0;
char NetRdC = 0;
String NetCMDS = "";
String NetPARS = "";


unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;





#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

unsigned long tmi =0;

byte pp = 0 ;



void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg){
  int16_t x1, y1;
  uint16_t wid, ht;
  tft.setFont(f);
  tft.setCursor(x, y);
  tft.setTextColor(0x0000);
  tft.setTextSize(sz);
  tft.print(msg);
}





void setup() {
  //***************************TFT ********************
    tft.reset();
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    tft.begin(ID);
    tft.setRotation(1);
    tft.invertDisplay(true);
    tft.fillScreen(0xffff);
    showmsgXY(100, 220, 2, &FreeSans9pt7b, "TFT Display UNO");
    showmsgXY(170, 265, 2, &FreeSans9pt7b, "Mecco");
    col[0] = tft.color565(0, 0, 0);           //Nero
    col[1] = tft.color565(0, 0, 255);         //Blu
    col[2] = tft.color565(0, 255, 0);         //Verde  
    col[3] = tft.color565(0, 0xAA, 0xAA);     //Ciano
    col[4] = tft.color565(255, 0, 0);         //Rosso
    col[5] = tft.color565(0xAA, 0, 0xAA);     //Magenta
    col[6] = tft.color565(0xAA, 0x55, 0);     //Marrone
    col[7] = tft.color565(0xAA, 0xAA, 0xAA);  //Grigio Chiaro
    col[8] = tft.color565(0x55, 0x55, 0x55);  //Grigio Scuro
    col[9] = tft.color565(0x55, 0x55, 0xFF);  //Blu Luminoso
    col[10] = tft.color565(0x55, 0xFF, 0x55); //Verde Luminoso
    col[11] = tft.color565(0x55, 0xFF, 0xFF); //Ciano Luminoso
    col[12] = tft.color565(0xFF, 0x55, 0x55); //Rosso Luminoso
    col[13] = tft.color565(0xFF, 0x55, 0xFF); //Magenta Luminoso
    col[14] = tft.color565(0xFF, 0xFF, 0x55); //Giallo Luminoso
    col[15] = tft.color565(255, 255, 255);    //Bianco
//********************************************************

  // Open serial communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
}


void loop() {
  
   }//***************************************   END   ************************************************************************
//****************************************************************************************************************************
//****************************************************************************************************************************
// Functions *****************************************************************************************************************   
