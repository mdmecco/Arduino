#include <stdint.h>
#include <SPI.h>
#include <Ethernet.h>
#include <TouchScreen.h>


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

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 14);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);






unsigned long tmi =0;

byte pp = 0 ;




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
  
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {

    pp=pp+1;
    int i=pp;

    //TFT Cercui che ruota
    //for (int i = 8; i > 0; i--) {
        tft.fillCircle(240 + 40 * (cos(-i * PI / 4)), 120 + 40 * (sin(-i * PI / 4)), 10, col[0]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 1)*PI / 4)), 120 + 40 * (sin(-(i + 1)*PI / 4)), 10, col[1]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 2)*PI / 4)), 120 + 40 * (sin(-(i + 2)*PI / 4)), 10, col[2]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 3)*PI / 4)), 120 + 40 * (sin(-(i + 3)*PI / 4)), 10, col[3]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 4)*PI / 4)), 120 + 40 * (sin(-(i + 4)*PI / 4)), 10, col[4]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 5)*PI / 4)), 120 + 40 * (sin(-(i + 5)*PI / 4)), 10, col[5]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 6)*PI / 4)), 120 + 40 * (sin(-(i + 6)*PI / 4)), 10, col[6]); delay(15);
        tft.fillCircle(240 + 40 * (cos(-(i + 7)*PI / 4)), 120 + 40 * (sin(-(i + 7)*PI / 4)), 10, col[7]); delay(15);
    //}



  
  
  showmsgXY(pp+50, 265, 2, &FreeSans9pt7b, "Mecco");

  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          /*for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          */
          
          TSPoint p = ts.getPoint();
          //if (p.z > ts.pressureThreshhold) {
             client.print("X = "); client.print(p.x);
             client.println("<br />");
             client.print("\tY = "); client.print(p.y);
             client.println("<br />");
             client.print("\tPressure = "); client.println(p.z);
             client.println("<br />");
         // }
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}



void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg){
  int16_t x1, y1;
  uint16_t wid, ht;
  tft.setFont(f);
  tft.setCursor(x, y);
  tft.setTextColor(0x0000);
  tft.setTextSize(sz);
  tft.print(msg);
}
