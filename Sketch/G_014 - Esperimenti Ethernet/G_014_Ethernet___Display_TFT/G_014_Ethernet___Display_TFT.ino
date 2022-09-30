#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <stdint.h>
#include <TouchScreen.h>
#include <SPI.h>
#include <Ethernet.h>

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

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 14);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


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
  //Serial.println("Ethernet WebServer Example");

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
  //server.begin();
  //Serial.print("server is at ");
  //Serial.println(Ethernet.localIP());


  OTAActive=false;
  
}


void loop() {

//**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
//********** SERVER ****************************************************************

  if (Ethernet.linkStatus() != LinkOFF) {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {

    switch (NetMas){
      case 0:
        client = server.available();
        if ((client) & (client.connected())){ // in ricezione
          NetMas=5;
          NetTo=millis()+3000;
        }
        break;
      case 5:
        if (client.available()){
          NetRdC=client.read();
          if(NetRdC=='<'){ // comando <CMD-PAR>
            NetMas=10;
            NetTo=millis()+100;     
          }else if(NetRdC=='G'){
            NetMas=50;     
          }else if(NetRdC=='P'){
            NetMas=50;     
          }
        }
        if (millis()>NetTo){
          NetMas=200; // funzione del timeout di ricezione
        }
        break;
      case 10:
        NetCMDS=client.readStringUntil('-');//
        NetPARS=client.readStringUntil('>');//
        if (millis()>NetTo){
          NetMas=200; // funzione del timeout di ricezione
        }
        NetMas=20;
        break;
      case 20:
        NetMas=0;
        client.stop();
        if (NetCMDS=="F1"){
          Serial.print(F("<M1-ACT-01>"));
        }else if (NetCMDS=="F2"){
          Serial.print(F("<M1-ACT-02>"));
        }else if (NetCMDS=="F3"){
          Serial.print(F("<M1-ACT-03>"));
        }else if (NetCMDS=="F4"){
          Serial.print(F("<M1-ACT-04>"));
        }else if (NetCMDS=="LAMP"){
          //ETH484Sw(3);
        }else if (NetCMDS=="APRI"){
          //ETH484Sw(0);  
        }else if (NetCMDS=="CHIUDI"){
          //ETH484Sw(1);  
        }else if (NetCMDS=="LUCEP"){
          //ETH484Sw(2);  
        }else if (NetCMDS=="MyIP"){
          //MyPublicIP();  
        }
        break;
      case 50:
        {
          NetTo=millis()+500;
          NetCMDS=client.readStringUntil('\r');
          int io1=NetCMDS.indexOf("/SET?");
          int io2=0;
          if (io1 > 0){
            rp=true;
            io1=NetCMDS.indexOf("?",io1-1)+1;
            io2=NetCMDS.indexOf(" ",io1);
            S1=NetCMDS.substring(io1, io2);
            //Serial.println(S1);
            io1=0;
            while (io1>=0){
              io2=S1.indexOf("=", io1);
              S2=S1.substring(io1,io2);
              io1=S1.indexOf("&", io2);
              if (io1>0){
                S3=S1.substring(io2+1,io1);
                io1=io1+1;
              }else{
                S3=S1.substring(io2+1);
              }
    //          UpdateParameter(S2, S3);
            }
          }
    
    
    //*****************************************  OTA  attivazione web  *****************************      
          io1=NetCMDS.indexOf("/OTA");
          io2=0;
          if (io1 > 0){
            rp=true;
            OTAActive= !OTAActive;
            //ArduinoOTA.begin(OTAActive);
          }
    //**********************************************************************************************      
          
    
          
          io1=NetCMDS.indexOf("/THOME?");
          io2=0;
          if (io1 > 0){
            rp=true;
            io1=NetCMDS.indexOf("?",io1-1)+1;
            io2=NetCMDS.indexOf(" ",io1);
            S1=NetCMDS.substring(io1, io2);
            io1=0;
            io2=S1.indexOf("=", io1);
            S2=S1.substring(io1,io2);
            io1=S1.indexOf("&", io2);
            if (io1>0){
              S3=S1.substring(io2+1,io1);
              io1=io1+1;
            }else{
              S3=S1.substring(io2+1);
            }
            //THome=S3.toFloat();
            //EEPROM.update(0,THome);
            //EEPROM.commit();
            //PompaRefresh=0;
    
            //SendGMA(30,"<OFFMAURIZIO-000>");
          }
          
          io1=NetCMDS.indexOf("/CLEAR?");
          io2=0;
          if (io1 > 0){
            rp=true;
          }
          
          io1=NetCMDS.indexOf("/CLEARSTAT?");
          io2=0;
          if (io1 > 0){
            rp=true;
            //LifePMar=0;
            //LifePMau=0;
            //TimePMar=millis();
            //TimePMau=millis();
          }
    
          while (client.available()){
            /*
            char c = client.read();
          }
    
          //********************************************* pagina server inizio  *****************************************************
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
    
          //-------------------------------------------Parte serve per OTA ***********************************************************
          if (rp){
            if (!OTAActive){
              client.println(F("Refresh: 0;url=/"));  // reset the pages
            }
            rp=false;
          //}else{
          //  if (!OTAActive){
          //    client.println(F("Refresh: 5"));
          //  }
          }
          //****************************************************************************************************************************
          
          
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.print(F("<html><head><title>"));
          //client.print(WEBTITPAGE);
          client.println(F("</title></head>"));
          client.println(F("<body>"));
          client.print(F("<b style=""font-size:10px"">"));
          client.print(PRGVER);
          client.print(F("</b>"));
          if (OTAActive){
            client.print(F("<table style=""width:100%"" border=1> <tr><th width=100% align=""center""> OTA Active </th> </tr> </table> <hr width=100% size=4 color=0000FF> "));
          }
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""center"">"));
          //client.print(WiFi.SSID());
          client.print(F(":"));
          //client.print(WiFi.RSSI());
          client.println(F("</th>"));
          client.print(F("<th width=50% align=""center"">"));
          //client.print(STime(DaySec()));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
          //************************************************* Fine Header ******************************************************
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Temperatura:</th><th width=50% align=""left"">"));
          //client.print(String(Tem,1));
          //client.print("    Index:");
          //client.print(String(ArrayI));
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Umidita':</th><th width=50% align=""left"">"));
          //client.print(String(Hum,1));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
    
    
          //************************************************ input
          client.print(F("<form action=""THOME"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TCMAX-"">Temperatura cella:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-THOME-"" name=""-THOME-"" value="""));
          //client.print(String(THome,0));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************
    
        
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Orario pompa ON</th><th width=50% align=""left"">"));
          //client.print(STime(LstON));
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Orario pompa OFF</th><th width=50% align=""left"">"));
          //client.print(STime(LstOFF));
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Stato pompa</th><th width=50% align=""left"">"));
          //if (PompaOn){
          //  client.print("ACCESA");
          //}else{
            client.print("SPENTA");
          //}
          client.println(F("</th></tr>"));
          client.print(F("</table>"));
    
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Tempo lettura sensore ms</th><th width=50% align=""left"">"));
          //client.print(ReadTempTime);
          client.println(F("</th></tr>"));
          client.print(F("</table>"));
    
      
    
          // chiudi BODY e HTML
          client.println(F("\r\n</body>\r\n</html>"));
          delay(100);
          */
          NetMas=0; // funzione del timeout di ricezione
          client.stop();
        }
        break;
      case 200:
        client.stop();
        NetMas=0;
        break;
      default:
        NetMas=0;
    }
    }

    // *********************Dentro Rete
    
    
    
    //************************************ 
    }else{
    //************************  FUORI RETE CAVO SCONNESSO *******************************************************************


    }

    //************************************  Eseguito sempre, con o senza rete ************************************************
  





  
   }//***************************************   END   ************************************************************************
//****************************************************************************************************************************
//****************************************************************************************************************************
// Functions *****************************************************************************************************************   



void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg){
  int16_t x1, y1;
  uint16_t wid, ht;
  tft.setFont(f);
  tft.setCursor(x, y);
  tft.setTextColor(0x0000);
  tft.setTextSize(sz);
  tft.print(msg);
}




void GetTime() {
  /* WiFiClient WcGMA;
  String Ln1 = "";
  unsigned long TT = 0;
  if (WcGMA.connect("www.mdmecco.it", 80)) {
    WcGMA.print(F("GET /ghelfa/time.php HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    WcGMA.flush();
    //delay(150);
    Ln1 = WcGMA.readStringUntil('<');
    Ln1 = WcGMA.readStringUntil('>');
    TT = atol(Ln1.c_str());
    if (TT > 0) {
      DayTimeB = true;
      DayTimeR = millis() + 28800000;
    } else {
      DayTimeB = false;
      DayTimeR = millis() + 600000;
    }
  } else {
    if (DayTimeS = 0) {
      DayTimeB = false;
      DayTimeR = millis() + 600000;
    } else {
      DayTimeB = true;
      DayTimeR = millis() + 600000;
    }
  }
  WcGMA.stop();
  DayTimeS = TT - ((millis() / 1000) % 86400); */
}

unsigned long DaySec() {
  if (DayTimeB) {
    return (((millis() / 1000) + DayTimeS) % 86400);
  } else {
    return 0;
  }
}
