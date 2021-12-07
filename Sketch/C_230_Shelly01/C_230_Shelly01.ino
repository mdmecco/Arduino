#include <ESP_EEPROM.h>
#include <ArduinoOTA.h>
#include "A:\libraries\ESP8266WiFi\src\ESP8266WiFi.h"
#include "a:\libmie\mecco1.h"
#include "a:\libmie\pulsanti.h"


#define IPFienile 10
#define IPNafta 27

#define OFFPOMPADEF "<OFFMAURIZIO-00>"
#define ONPOMPADEF "<ONMAURIZIO-00>"
#define WEBTITPAGE "Shelly 033"
#define PRGVER "2021-01-16 V1.0"
#define LogFile "GET /ghelfa/log.php?FileTS=Shelly033&StatoR="
#define MySIp 33
#define HOSTNAME "Shelly_033"


//const IPAddress staticIP(192, 168, 1, MySIp);
const IPAddress staticIP(172, 20, 217, 230);

const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";
//const char* ssid3 = "DGhelfa";
//const char* password3 = "Lamborghini";

const char* ssid3 = "WIFI_PDA";
const char* password3 = "s4nf3l1c3";


IPAddress GMA(192, 168, 1, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

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


unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;

String S1;
String S2;
String S3;
bool rp=false;
bool OTAActive=false;


unsigned long TP1=0;              //tempo antirimbalzo
byte BP1=B00100000;                       //Stato 
unsigned long TOnActP1=1800000;   //Tempo massimo acceso
unsigned long TOnP1=3000;            //Tempo start attivazione

//void ProcBtn (BP1, TP1, TOnActP1, TOnP1)

void setup() { 
  EEPROM.begin(512);
  OTAActive=false;
  WiFi.hostname(HOSTNAME);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);  
}

void loop() {
  //***************** CONNESSIONE WIFI ******************************************************

  switch (WifiMas){
    case 100:
      if (OTAActive){
      ArduinoOTA.handle();  
      }
      if (WiFi.status() != WL_CONNECTED) {
        WifiMas = 10;
      }
      break;    
    case 0:  
      WifiMas = 1;
      break;
    case 1:
      if (WIFIScan() > 0) {
        WiFi.config(staticIP, gateway, subnet, DNS);
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
      GetTime();
      break;
    case 7:
      server.begin(false);
      WifiT0 = millis() + 5000;
      WifiMas = 11;
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
      WifiMas = 0;
  }
  //**********************************************************************************

//********** SERVER ****************************************************************
  if (WifiMas ==100) { // Rete attiva e connessa
    switch (NetMas){
      case 0:
        client = server.available();
        client.setNoDelay(true);
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
            ArduinoOTA.begin(OTAActive);
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
//            THome=S3.toFloat();
//            EEPROM.write(0,THome);
//            EEPROM.commit();
    
//            SendGMA(30,"<OFFMAURIZIO-000>");
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
          }else{
            if (!OTAActive){
              client.println(F("Refresh: 5"));
            }
          }
          //****************************************************************************************************************************
          
          
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.print(F("<html><head><title>"));
          client.print(WEBTITPAGE);
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
          client.print(WiFi.SSID());
          client.print(F(":"));
          client.print(WiFi.RSSI());
          client.println(F("</th>"));
          client.print(F("<th width=50% align=""center"">"));
          client.print(STime(DaySec()));
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
          
//          client.print(String(Hum,1));

          client.println(F("</th>"));
          client.print(F("</tr></table>"));
    
    
          //************************************************ input
          client.print(F("<form action=""THOME"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TCMAX-"">Temperatura casa:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-THOME-"" name=""-THOME-"" value="""));
//          client.print(String(THome,0));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************
    
    
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Orario spedizione pompa ON</th><th width=50% align=""left"">"));
          //client.print(STime(LstSentON));
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Orario spedizione pompa OFF</th><th width=50% align=""left"">"));
          //client.print(STime(LstSentOFF));
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Stato pompa lato ambiente</th><th width=50% align=""left"">"));
          //if (PompaOn){
          //  client.print("ACCESA");
          //}else{
          //  client.print("SPENTA");
          //}
          client.println(F("</th></tr>"));
          client.print(F("</table>"));
    
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> BIT </th><th width=50% align=""left"">"));
          //client.print(STime(LstON));
          client.println(String(BP1, BIN));
          client.println(F("</th></tr>"));
          client.print(F("</table>"));
    
    
          // chiudi BODY e HTML
          client.println(F("\r\n</body>\r\n</html>"));
          delay(100);
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
//***********************************************************************
//*********************** codice dentro rete  ***************************
//***********************************************************************
    
    if (millis() > DayTimeR) {
      GetTime();
    }



  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************


  bitWrite(BP1,0,digitalRead(5));
  ProcBtn (BP1, TP1, TOnActP1, TOnP1);
  digitalWrite(4,bitRead(BP1,1));


//**************************************************************************************
}
//********** fine MAIN  ***************************************************************
//**************************************************************************************
//------------------------------------------------- Funzioni --------------------------


bool SendGMA(byte IpA, String MyCmd){
  String SS;
  WiFiClient WcGMA;
  GMA[3]=IpA;
  if (WcGMA.connect(GMA, 80)){
    WcGMA.print(MyCmd);
  }
  WcGMA.flush();
  if (WcGMA){
    while (WcGMA.connected()){
      SS=WcGMA.readStringUntil('>');//
      return (SS='<OK-00');
    }
    WcGMA.flush();
    WcGMA.stop();
  }
}

void LogCaldaia(String TT){
  WiFiClient WcGMA;
  if (WcGMA.connect("www.mdmecco.it", 80)) {
    WcGMA.print(LogFile);
    WcGMA.print(TT);
    WcGMA.print(F(" HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    WcGMA.flush();
  }
  WcGMA.stop();
}


void GetTime() {
  WiFiClient WcGMA;
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
  DayTimeS = TT - ((millis() / 1000) % 86400);
}

unsigned long DaySec() {
  if (DayTimeB) {
    return (((millis() / 1000) + DayTimeS) % 86400);
  } else {
    return 0;
  }
}


int WIFIScan() {                                                                           //Scan delle Wifi
  int RssV = -1000;
  int H=-1;
  WId = 0;
  WiFi.scanDelete();
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
  } else {
    for (int i = 0; i < n; ++i) {
      //tft.print(WiFi.SSID(i));
      //tft.print(" -->");
      //tft.print(WiFi.RSSI(i));
      //tft.println("dB");
      
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
 
  return WId;
}
