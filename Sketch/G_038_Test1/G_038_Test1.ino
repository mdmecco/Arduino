#include <ESP_EEPROM.h> //https://github.com/esp8266/Arduino/blob/master/libraries/EEPROM/EEPROM.h

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>


#include "a:\libmie\pulsanti.h"
#include "a:\libmie\mecco1.h"


#define WEBTITPAGE "Test1"
#define PRGVER "2022-05-23 V1.0"
#define MySIp 38

// ********************** DEFINIZIONE MODULO *************************************
const IPAddress staticIP(192, 168, 1, MySIp);
const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";
const char* ssid3 = "DGhelfa";
const char* password3 = "Lamborghini";

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
bool VHeater = false;
bool VHumid =false;

String WiFiSt = "";
char WiFiCh = 0;


// Variabili per gestione rete ed ambiente
byte NetMas = 0;
unsigned long NetTo = 0;
char NetRdC = 0;
String NetCMDS = "";
String NetPARS = "";


unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;
unsigned long DayDate=0;

#define ArrayLenght 255
float TmpArray[ArrayLenght]={0};
unsigned long ArrayTime=0;
unsigned int ArrayI=0;
#define ArrayRefresh 600000

String S1;
String S2;
String S3;
bool rp=false;
bool OTAActive=false;

/*
  #define L1 15
  #define L2 13
  #define L3 12
  #define L4 14
*/

  #define L1 2
  #define L2 0
  #define L3 4
  #define L4 5

  #define Bl 15



unsigned long TL1 =0;
unsigned long TL2 =0;
unsigned long TL3 =0;
unsigned long TL4 =0;
unsigned long TBl =0; //delay lampeggi




byte BL1=0;
byte BL2=0;
byte BL3=0;
byte BL4=0;

unsigned long tBl=0;  //delay lampeggi programmato
byte mBl=0; // macchina a stati dei lampeggi
byte nBl=0; // numero di lampeggi da fare

void setup() { 
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);
//  Serial.begin(9600);

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(Bl, OUTPUT);

  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  digitalWrite(L4, LOW);
  digitalWrite(Bl, HIGH);  // Si spegne quando si fa il collegamento con la wifi
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
      digitalWrite(Bl, LOW);   // da qui sono collegato con la wifi
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
      digitalWrite(Bl, HIGH);      
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
        if (NetCMDS=="STATUS"){
          byte ST=0;
          if (BL1!=0){
            ST=ST+1;
          }
          if (BL2!=0){
            ST=ST+2;
          }
          if (BL3!=0){
            ST=ST+4;
          }
          if (BL4!=0){
            ST=ST+8;
          }
          client.print("<STATUS-");
          String dd = String(ST);
          client.print(dd);
          client.print(">");
          
        }else if (NetCMDS=="L1"){
          mBl=2;
          nBl=3;
          tBl=50;
          if (BL1!=0){
            BL1=7;
          }else{
            TL1=GetValue(NetPARS)+millis();
            BL1=6;
          }
        }else if (NetCMDS=="L2"){
          mBl=2;
          nBl=3;
          tBl=50;
          if (BL2!=0){
            BL2=7;
          }else{
            TL2=GetValue(NetPARS)+millis();
            BL2=6;
          }
        }else if (NetCMDS=="L3"){
          mBl=2;
          nBl=3;
          tBl=50;
          if (BL3!=0){
            BL3=7;
          }else{
            TL3=GetValue(NetPARS)+millis();
            BL3=6;
          }
        }else if (NetCMDS=="L4"){
          mBl=2;
          nBl=3;
          tBl=50;
          if (BL4!=0){
            BL4=7;
          }else{
            TL4=GetValue(NetPARS)+millis();
            BL4=6;
          }
        }
        NetMas=0;
        client.stop();
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

          //***************************************L1
          io1=NetCMDS.indexOf("/L1 ");
          mBl=2;
          nBl=5;
          tBl=50;
          io2=0;
          if (io1 > 0){
            rp=true;
            if (BL1!=0){
              BL1=7;
            }else{
              TL1=10000 + millis();
              BL1=6;
            }
          } // ******************************************************************
          
          //***************************************L2
          io1=NetCMDS.indexOf("/L2 ");
          mBl=2;
          nBl=5;
          tBl=50;
          io2=0;
          if (io1 > 0){
            rp=true;
            if (BL2!=0){
              BL2=7;
            }else{
              TL2=10000 + millis();
              BL2=6;
            }
          } // ******************************************************************


          //***************************************L3
          io1=NetCMDS.indexOf("/L3 ");
          mBl=2;
          nBl=5;
          tBl=50;
          io2=0;
          if (io1 > 0){
            rp=true;
            if (BL3!=0){
              BL3=7;
            }else{
              TL3=10000 + millis();
              BL3=6;
            }
          } // ******************************************************************

          //***************************************L4
          io1=NetCMDS.indexOf("/L4 ");
          mBl=2;
          nBl=5;
          tBl=50;
          io2=0;
          if (io1 > 0){
            rp=true;
            if (BL4!=0){
              BL4=7;
            }else{
              TL4=10000 + millis();
              BL4=6;
            }
          } // ******************************************************************


          while (client.available()){
            char c = client.read();
          }
    
          //********************************************* pagina server inizio  *****************************************************
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
    
          //------------------------------------------- Refresh  ***********************************************************
          if (rp){
              client.println(F("Refresh: 0;url=/"));  // reset the pages
            rp=false;
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
          client.print(DayDate);
          client.print(" - " );
          client.print(STime(DaySec()));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
          //************************************************* Fine Header ******************************************************
    
        //tabelle inizio

          //**********************************************  L1  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL1==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L1\" >___L1___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          //**********************************************  L2  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL2==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L2\" >___L2___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            


          //**********************************************  L3  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL3==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L3\" >___L3___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          //**********************************************  L4  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL4==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L4\" >___L4___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          




            
            client.println(F("</body>\r\n</html>"));
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
      GetDate();
    }
    

  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************
//L1
  if (BL1==5){ //controlla se spegnere
    if ( millis() > TL1 ){
      digitalWrite(L1, LOW);
      BL1=0;
    }
  }else if (BL1==6){  // Accendi la luce
    digitalWrite(L1, HIGH);
    BL1=5;
  }else if (BL1==7){  // Spegni Subito
    digitalWrite(L1, LOW);
    BL1=0;
  }

//L2
  if (BL2==5){ //controlla se spegnere
    if ( millis() > TL2 ){
      digitalWrite(L2, LOW);
      BL2=0;
    }
  }else if (BL2==6){  // Accendi la luce
    digitalWrite(L2, HIGH);
    BL2=5;
  }else if (BL2==7){  // Spegni Subito
    digitalWrite(L2, LOW);
    BL2=0;
  }


//L3
  if (BL3==5){ //controlla se spegnere
    if ( millis() > TL3 ){
      digitalWrite(L3, LOW);
      BL3=0;
    }
  }else if (BL3==6){  // Accendi la luce
    digitalWrite(L3, HIGH);
    BL3=5;
  }else if (BL3==7){  // Spegni Subito
    digitalWrite(L3, LOW);
    BL3=0;
  }

//L4
  if (BL4==5){ //controlla se spegnere
    if ( millis() > TL4 ){
      digitalWrite(L4, LOW);
      BL4=0;
    }
  }else if (BL4==6){  // Accendi la luce
    digitalWrite(L4, HIGH);
    BL4=5;
  }else if (BL4==7){  // Spegni Subito
    digitalWrite(L4, LOW);
    BL4=0;
  }



// gestore del lampeggio del led di servizio
  if (mBl != 0){
    if (millis() > TBl){
        if (mBl==2){
          digitalWrite(Bl, HIGH);
          if (nBl > 0){
            nBl= nBl-1;
          }
          mBl=1;
          TBl=millis()+tBl;
        }else if (mBl==1){
          digitalWrite(Bl, LOW);
          TBl=millis()+tBl;
          mBl=2;
          if (nBl == 0){
            mBl=0;
          }
        }else{
          mBl=0;
          nBl=0;
          tBl=1000;
        }
    }
  }



 
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
      return (SS=="<OK-00");
    }
    WcGMA.flush();
    WcGMA.stop();
  }
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



void GetDate() {
  WiFiClient WcGMA;
  String Ln1 = "";
  unsigned long TT = 0;
  if (WcGMA.connect("www.mdmecco.it", 80)) {
    WcGMA.print(F("GET /ghelfa/day.php HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    WcGMA.flush();
    //delay(150);
    Ln1 = WcGMA.readStringUntil('<');
    Ln1 = WcGMA.readStringUntil('>');
    TT = atol(Ln1.c_str());
    DayDate=TT;
  }
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



word EEPR(byte Id){
  byte Eby1=0;
  byte Eby2=0;
  unsigned int Ev=0;
  Ev=word(Eby2,Eby1);
  return Ev;  
}

void EEPW(byte Id, word EvV){
  byte Eby1=0;
  byte Eby2=0;
  Eby1=lowByte(EvV);
  Eby2=highByte(EvV);
}


unsigned long GetValue (String Vs){
  unsigned long dd=0;
  dd= Vs.toInt();
  dd=dd*1000;
  return dd;
}



String BtnColor (int idCol){
  if (idCol == 0){
    return "#22ff22";
  }else{
    return "red";
  }
}
