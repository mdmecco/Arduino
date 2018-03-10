


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include <UnoWiFiDevEd.h>
#include <LiquidCrystal_I2C.h>



#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
                          // Address can be changed by soldering A0, A1, or A2
                          // Default is 0x27

// map the pin configuration of LCD backpack for the LiquidCristal class
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define INTERRUPT_INPUT 2
#define BtnGiallo 0 // reset il total counter
#define BtnRosso 75 // cambia visualizzazione
#define BtnMilli 200



LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);


// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


long C1 = 0;
int T1 = 0;
int T2 = 0;
long T3 = 0;


void setup() {
  ss.begin(GPSBaud);
  
  Wifi.begin();
  Wifi.println("Web Server is up");
  lcd.begin(20,4);        // 20 columns by 4 rows on display
  lcd.setBacklight(HIGH); // Turn on backlight, LOW for off
}
void loop() {

  while(Wifi.available()){
    process(Wifi);
  }

T1=analogRead(0);
lcd.setCursor (0,2);
if (T1<100) {
  T3=millis()+BtnMilli;  
  if (T2 !=1){ //BtnRosso) && (T2 != BtnGiallo)){
    if (T1>50){
      T2=1;
      lcd.print(F("Bottone Rosso"));
    }else{
      T2=1;
      lcd.print(F("Bottone Giallo"));    
    }    
  }
}else{
  if (millis() > T3){
    T2=0;
    lcd.print(F("              "));
  }
}



  
if (ss.available() > 0){ // any data coming in?
    if (gps.encode(ss.read())){
      lcd.setCursor (0,0);
      if (gps.date.isValid()){
        if (gps.date.day() < 10) lcd.print(F("0"));
        lcd.print(gps.date.day());
        lcd.print(F("/"));
        if (gps.date.month() < 10) lcd.print(F("0"));
        lcd.print(gps.date.month());
        lcd.print(F("/"));
        lcd.print(gps.date.year());
        lcd.print(F("  "));
      }
      if (gps.time.isValid()){
        if (gps.time.hour() < 10) lcd.print(F("0"));
        lcd.print(gps.time.hour());
        lcd.print(F(":"));
        if (gps.time.minute() < 10) lcd.print(F("0"));
        lcd.print(gps.time.minute());
        lcd.print(F(":"));
        if (gps.time.second() < 10) lcd.print(F("0"));
        lcd.print(gps.time.second());
      }
      lcd.setCursor (0,3);
      if (gps.location.isValid()){
        lcd.print(gps.location.lat(), 4);
        lcd.print(F(","));
        lcd.print(gps.location.lng(), 4);
        lcd.print(F(","));
        }
   }
 }
}

void process(WifiData client) {
  // read the command
  String command = client.readStringUntil('/');
  String miocmd= client.  

  if (command == "webserver") {
    WebServer(client);
  }

  if (command == "NEO") {
    WebServer(client);
  }


  
}
void WebServer(WifiData client) {
  C1 = C1 + 1;
  lcd.setCursor (0,1);
  lcd.print(C1);
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println("Refresh: 20");  // refresh the page automatically every  sec
  client.println();
  client.println("<html>");
  client.println("<head> <title>ANALOG</title> </head>");
  client.print("<body>");

  for(int analogChannel = 0; analogChannel < 4; analogChannel++) {
    int sensorReading = analogRead(analogChannel);
    client.print("analog input  ");
    client.print(analogChannel);
    client.print(" is:");
    client.print(sensorReading);
    client.print("<br/>");
  }

    client.print("analog input  5 is:");
    client.print(C1);
    client.print("<br/>");

    client.print("analog input  6 is:");
    client.print(T3);
    client.print("<br/>");



  client.print("</body>");
  client.println("</html>");
  client.print(DELIMITER); // very important to end the communication !!!
}
