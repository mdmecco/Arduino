// https://www.netspotapp.com/it/wifi-signal-strength-and-its-impact.html

//#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "DHT.h"
#define DHTPIN 0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>


#include <Adafruit_GFX.h>
// Core graphics library
//#include <Adafruit_TFTLCD.h>
// Hardware-specific library
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>


#define IPFienile 10
#define IPNafta 27

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



const IPAddress staticIP(192, 168, 1, 28);
const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";
const char* ssid3 = "DGhelfa";
const char* password3 = "Lamborghini";

IPAddress GMA(192, 168, 1, 0);
IPAddress ETH(192, 168, 1, 6);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress MD(31, 11, 33, 162);
IPAddress DNS(8, 8, 8, 8);
WiFiServer server(80);
WiFiClient client;
byte WifiMas = 0; //Macchina a stati per la connessione WIFI
unsigned long WifiT0 = 0; // tempi di attesa per la connessione wifi
unsigned long WifiT1 = 0;
unsigned int WId = 0;
byte WiFiClMAS = 0;
unsigned long WiFiClTo = 0;
bool WiFiBo = false;
String WiFiSt = "";
char WiFiCh = 0;

byte NetMas = 0;
unsigned long NetTo = 0;
char NetRdC = 0;
String NetCMDS = "";
String NetPARS = "";

byte ETHLed = 0;
unsigned int ETHAdc[4];

unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;


// https://techtutorialsx.com/2017/04/26/esp8266-flash-chip-information-functions/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

String S1;
String S2;
String S3;
bool rp=false;
bool OTAActive=false;

float Tem=0;
float Hum=0;


// Color definitions
#define BLACK 0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

bool dstart = false;
unsigned long TimePrint =0;
unsigned long HH=0;
unsigned long MM=0;
bool InitClear=true;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

byte SeqL=0;
unsigned long SPT1=0;
unsigned int SPv=0;
unsigned int SPi=0;
unsigned int SPb=0;
unsigned int SPKey=0;
bool SPKeyB=false;

//*********************************** UDP ************************************
WiFiUDP MUdp;
int localUdpPort=5240;
int packetSize=0;
char incomingPacket[256];
char T[]={0,0,0,0,0};
//****************************************************************************

void setup() { 

//************************************************************ Display *****************************
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  dht.begin();
  tft.fillRect(0,0,160,128, 0);
  tft.setTextSize(4);
  tft.setCursor(28,40);
  tft.print("G028");
  delay(2000);
  tft.fillRect(0,0,160,128, 0);

  pinMode(12, INPUT);
  pinMode(16, OUTPUT);
  pinMode(15, OUTPUT);

  OTAActive=false;
}

void loop() {
  //***************** CONNESSIONE WIFI ******************************************************
  
  switch (WifiMas){
    case 100:
      if (OTAActive) {
        ArduinoOTA.handle();
      }
      if (WiFi.status() != WL_CONNECTED) {
        WifiMas = 10;
      }
      break;
    case 0:
      WifiMas = 1;
      WifiT0 = millis() + 8000;
      break;
    case 1:
      if (WIFIScan() > 0) {
        WiFi.config(staticIP, gateway, subnet, DNS);
        WiFi.hostname("E026");
        if (WId == 1) {
          WiFi.begin(ssid1, password1);
          WifiMas = 4;
        } else if (WId == 2) {
          WiFi.begin(ssid2, password2);
          WifiMas = 4;
        } else if (WId == 3) {
          WiFi.begin(ssid3, password3);
          WifiMas = 4;
        } else {
          WifiMas = 2;
        }
      }
      break;
    case 2:
      WiFi.disconnect();
      WifiMas = 10;
      break;
    case 4:
      WifiT0 = millis() + 6000;
      WifiMas = 5;
      break;
    case 5:
      if (WiFi.status() == WL_CONNECTED) {
        WifiMas = 6;
      }
      if (millis() > WifiT0 ) {
        WifiMas = 10;
      }
      break;
    case 6:
      server.begin();
      WifiMas = 100;
      tft.println("Connected...");
      GetTime();
      tft.println(DaySec());
      TimePrint=millis()+3000;
      break;
    case 10:
      WifiT0 = millis() + 5000;
      WifiMas = 11;
      InitClear=false;
      break;
    case 11:
      if (millis() > WifiT0) {
        ArduinoOTA.begin(false);
        server.close();
        WiFi.disconnect();
        WifiMas = 0;
      }
      break;
    default:
      WifiMas = 100;
      break;
    }
  //**********************************************************************************

  //********** SERVER ****************************************************************
//********** SERVER ****************************************************************
  if (WifiMas ==100) { // Rete attiva e connessa
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
        }else if (NetCMDS=="STATUS"){

          
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
            Serial.println(S1);
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
    
          io1=NetCMDS.indexOf("/TCMAX?");
          io2=0;
          if (io1 > 0){
            rp=true;
            io1=NetCMDS.indexOf("?",io1-1)+1;
            io2=NetCMDS.indexOf(" ",io1);
            S1=NetCMDS.substring(io1, io2);
            Serial.println(S1);
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
     //       TCMax=S3.toFloat();
     //       EEPROM.write(48,TCMax);
     //       EEPROM.commit();
          }
          
    //*****************************************  OTA  attivazione web  *****************************      
          io1=NetCMDS.indexOf("/OTA");
          io2=0;
          if (io1 > 0){
            rp=true;
            OTAActive= !OTAActive;
            ArduinoOTA.begin(OTAActive);
          }
    //**********************************************************************************************      
          
          io1=NetCMDS.indexOf("/CLEARSTAT?");
          io2=0;
          if (io1 > 0){
            rp=true;
          }
    
          io1=NetCMDS.indexOf("/SPEGNIMARIO?");
          io2=0;
          if (io1 > 0){
            rp=true;
            //PMar=false;
            //StMar=true;
          }
          
          io1=NetCMDS.indexOf("/SPEGNIMAURIZIO?");
          io2=0;
          if (io1 > 0){
            rp=true;
            //PMau=false;
            //StMau=true;
          }
    
          while (client.available()){
            char c = client.read();
          }
    
          //********************************************* pagina server inizio  *****************************************************
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          if (rp){
            if (!OTAActive){
              client.println(F("Refresh: 0;url=/"));  // reset the pages
              InitClear=false;
            }
            rp=false;
          }else{
            if (!OTAActive){
              //client.println(F("Refresh: 5"));
            }
          }
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.println(F("<html><head><title>Ghelfa Remote G028 </title></head>"));
          client.println(F("<body>"));
          if (OTAActive){
            client.print(F("<table style=""width:100%"" border=1> <tr><th width=100% align=""center""> OTA Active </th> </tr> </table> <hr width=100% size=4 color=0000FF> "));
          }
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""center"">"));
          client.print(WiFi.SSID());
          client.print(F(":"));
          client.print(WiFi.RSSI());
          client.println(F("</th>"));
          client.print(F("<th width=50% align=""center"">"));
          client.print(STime(DaySec()));
          client.println(F("</th></tr>"));
          client.print(F("</table>"));
          //************************************************* Fine Header ******************************************************
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Temperatura:</th><th width=50% align=""left"">"));
          client.print(String(Tem,1));
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Umidita':</th><th width=50% align=""left"">"));
          client.print(String(Hum,1));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
    
          // chiudi BODY e HTML
          client.println(F("\r\n</body>\r\n</html>"));
          delay(100);
          NetMas=0; // funzione del timeout di ricezione
          client.stop();
          break;
        }
      case 200:
        client.stop();
        NetMas=0;
        break;
      default:
        client.stop();
        NetMas=0;
        break;
    }
  
//**************************************************************************************  
//**************************************************************************************  
//**************************************************************************************  
// ************************   Codice dentro rete ***************************************


  if (millis() > TimePrint) {  
    if (InitClear){
      tft.fillRect(0,0,160,128, 0);
      tft.fillRect(0,0,160,11, 0xBBBBFF);
      InitClear=false;
    }
    TimePrint=millis()+5000;
    tft.setTextSize(1);    
    tft.setTextColor(BLACK, 0xBBBBFF);
    tft.setCursor(70,2);
    tft.print(STime(DaySec()));
    if (OTAActive) {
      tft.setTextColor(RED, WHITE);
      tft.print("OTA");
      tft.setTextColor(BLACK, 0xBBBBFF);
    }
    tft.setCursor(0,2);
    tft.print (WiFi.SSID());
    tft.setCursor(128,2);
    tft.print (WiFi.RSSI());
    tft.print("dB");
  }

  if ((SPb==0)&&(SPi==1)) {
    SPKeyB=false;
  }
  
  if (SPi==1){
    if (SPKeyB==false){
      switch (SPb) { 
        case 2:
          SPKeyB=true;
          SendUDP(10,"L1",0);
          break;
        case 8:
          SPKeyB=true;
          SendUDP(10,"L5",0);
          break;
        case 4:
          SPKeyB=true;
          SendUDP(10,"L6",0);
          break;
        case 64:
          SPKeyB=true;
          SendUDP(10,"L7",0);
          break;
        case 32:
          SPKeyB=true;
          SendUDP(10,"L8",0);
          break;
        case 512:
          SPKeyB=true;
          SendUDP(10,"L2",0);
          break;
        case 1024:
          SPKeyB=true;
          SendUDP(13,"L1",0);
          break;
        case 2048:
          SPKeyB=true;
          SendUDP(37,"L1",0);
          SendUDP(37,"L2",0);
          break;
        case 4096:
          SPKeyB=true;
          SendUDP(13,"L2",0);
          break;

        case 32768:
          SPKeyB=true;
          SendUDP(37,"L3",0);
          SendUDP(37,"L4",0);
          break;
          
        default:
          break;
      }
    }
  }

  if  (!InitClear) {
    switch (SeqL){
      case 0:
        Hum = dht.readHumidity();
        SeqL=1;
        break;
      case 1:
        Tem = dht.readTemperature();
        SeqL=2;
        break;
      case 2:
        tft.setTextSize(2);
        tft.setTextColor(CYAN);
        tft.setCursor(0,20);
        tft.print("T:");
        tft.setTextColor(WHITE, BLACK);
        tft.print(String(Tem,1));
        tft.print("C");
        SeqL=3;
        break;
      case 3:
        tft.setTextSize(2);
        tft.setTextColor(CYAN);
        tft.setCursor(0,40);
        tft.print("H:");
        tft.setTextColor(WHITE, BLACK);
        tft.print(String(Hum,1));
        tft.print("%");
        tft.fillRect(0,60, 160,3, BLUE);
        SeqL=0;
        break;
      default:
        SeqL=0;
        break;
    }
  }


  switch (SPi){
    case 0:
      tft.setCursor(0,80);
      tft.setTextSize(3);    
      tft.setTextColor(CYAN);
      tft.print("Key:");
      tft.setTextColor(WHITE, BLACK);
      tft.print(SPb);
      tft.print("     ");
      tft.setTextColor(WHITE, BLACK);
      tft.print("     ");
      
      digitalWrite(15, false);
      SPT1=millis()+1;
      SPi=1;
      break;
    case 1:
      if (millis() > SPT1){ 
        digitalWrite(15, true);
        SPT1=millis()+1;
        SPi=2;
        SPv=0;  
      }
      break;
    case 2:
      SPT1=millis()+1;
      digitalWrite(16, true);
      SPi=3;
      break;
    case 3:
      if (millis() > SPT1){ 
        digitalWrite(16,false);
        SPi=4;
      }
      break;
    case 4:
      bitWrite(SPKey, (15-SPv), !digitalRead(12));
      SPv += 1;
      if (SPv==15){
        SPb=SPKey;
        SPi=0;
      }else{
        SPi=2;
      }
      break;
    default:
      SPi=0;
      break;
   }




//**************************************************************************************  
  } //**********************************************************************************
//**************************************************************************************  
//**************************************************************************************  
// ************************   Codice fuori rete ****************************************













//**************************************************************************************
}//********** fine MAIN  ***************************************************************
//**************************************************************************************

//------------------------------------------------- Funzioni --------------------------

void SendGMA(byte IpA, String MyCmd){
  WiFiClient WcGMA;
  GMA[3]=IpA;
  if (WcGMA.connect(GMA, 80)){
    WcGMA.print(MyCmd);
  }
  WcGMA.flush();
  if (WcGMA){
    while (WcGMA.connected()){
      while (WcGMA.available()){
        char cll=WcGMA.read();
      }
    }
    WcGMA.flush();
    WcGMA.stop();
  }
}

void GetTime() {
  WiFiClient WcGMT;
  String Ln1 = "";
  unsigned long TT = 0;
  if (WcGMT.connect("www.mdmecco.it", 80)) {
    WcGMT.print(F("GET /ghelfa/time.php HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    WcGMT.flush();
    //delay(150);
    Ln1 = WcGMT.readStringUntil('<');
    Ln1 = WcGMT.readStringUntil('>');
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
  WcGMT.flush();
  WcGMT.stop();
  DayTimeS = TT - ((millis() / 1000) % 86400);
}

unsigned long DaySec() {
  if (DayTimeB) {
    return (((millis() / 1000) + DayTimeS) % 86400);
  } else {
    return 0;
  }
}



int WIFIScan() {             //Scan delle Wifi
  InitClear=true;
  int RssV = -1000;
  int H=-1;
  WId = 0;
  WiFi.scanDelete();
  int n = WiFi.scanNetworks();
  tft.fillRect(0,0,160,128, 0);
  tft.setCursor(0,5);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  
  if (n == 0) {
  } else {
    for (int i = 0; i < n; ++i) {
      tft.print(WiFi.SSID(i));
      tft.print(" -->");
      tft.print(WiFi.RSSI(i));
      tft.println("dB");
      
      if (WiFi.SSID(i) == "GGhelfa") {
        if (WiFi.RSSI(i) > RssV) {
          RssV = WiFi.RSSI(i);
          WId = 1;
          H=i;
        }
      }
      if (WiFi.SSID(i) == "WGhelfa") {
        if (WiFi.RSSI(i) > RssV) {
          RssV = WiFi.RSSI(i);
          WId = 2;
          H=i;
        }
      }
      if (WiFi.SSID(i) == "DGhelfa") {
        if (WiFi.RSSI(i) > RssV) {
          RssV = WiFi.RSSI(i);
          WId = 3;
          H=i;
        }
      }
    }
  }
  tft.println("");
  tft.print("use WiFi:");
  if (H != -1){
    tft.println(WiFi.SSID(H));
  }else{
    tft.println("-");
  }
  
  return WId;
}


String STime (unsigned long TTime){  // ****************  Converte in stringa orario il numero di secondi dalla mezzanotte, o in genere un tempo in secondi in HH:MM
  unsigned int HH=(TTime / 3600);
  unsigned int MM=(TTime % 3600)/60;
  String Tmp="";
  if (HH<10){
    Tmp="0";
    Tmp.concat(String(HH,DEC));
  }else{
    Tmp=(String(HH,DEC));
  }
  Tmp.concat(":");
  if (MM<10){
    Tmp.concat("0");
    Tmp.concat(String(MM,DEC));
  }else{
    Tmp.concat(String(MM,DEC));
  }
  return Tmp;  
}

long int StringToInt( String InS){
  long int dd=0;
  dd=atoi(InS.c_str());
  return dd;
}


void SendUDP(byte ToWho, String Cmd, unsigned long TOnTime){
  IPAddress IpU=(192,168,1,0);
  IpU[3]=ToWho;
  char D[8]={0,0,0,0,0,0,0,0};
  D[0]=Cmd.charAt(0);
  D[1]=Cmd.charAt(1);
  D[2]='-';
  D[3]='0';
  D[4]='0';
  D[5]='0';
  D[6]='0';
  D[7]='0';
  MUdp.beginPacket(IpU, localUdpPort );
  MUdp.write(D);
  MUdp.endPacket();
}
