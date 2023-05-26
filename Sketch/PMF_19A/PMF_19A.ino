#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "A:\Sketch\PMF_19A\LCD\LiquidCrystal_I2C.h"
#include "A:\libraries\TinyGPS\TinyGPS++.h"
#include "A:\libraries\AAMecco\pulsanti.h"



#define WEBTITPAGE "Flussimetro Mario"
#define PRGVER "2023-06-26"
#define MySIp 19

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


String S1;
String S2;
String S3;
bool rp=false;
bool OTAActive=false;

//************************************ Aggiunte per PMF ********************************************************************************

// map the pin configuration of LCD backpack for the LiquidCristal class
#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
                          // Address can be changed by soldering A0, A1, or A2
                          // Default is 0x27
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define INTERRUPT_INPUT 2
//LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);
//LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, 1);


unsigned long DelayDateDisplay=0;

#define PRosso 14
unsigned long TARR=0;              //momento di pressione antirimbalzo in millis()
byte SPR=B00000000;                       //Stato 
unsigned long TATR=0;         //Tempo start attivazione

#define PGiallo 12
unsigned long TARG=0;              //tempo antirimbalzo in millis()
byte SPG=B00000000;                       //Stato 
unsigned long TATG=0;         //Tempo start attivazione
unsigned long TOnActP=180;   //Tempo massimo acceso

int iMenu= 1 ;

// Coordinate e memoria coordinate
double Plat=44.872673;  //Posizione della Ghelfa
double Plng=11.098512;

// Gestione del flussimetro
bool FlowType=false; //Tipo visualizzazione del flusso
unsigned long pulse_counter2 = 0;
unsigned long pulse_counterT = 0;
volatile unsigned long PMFContatore = 0;
#define MSecCampionamento 2000 //Definisce ogni quanto tempo fare il refresh delle misure
unsigned long pulse_WaitT=2000;
#define TrCal  0.00631    //Conversione impulsi con litri
double lt_mt=0;

String StS="";
String StTot="";

bool WiFiEnabled=false;

void ICACHE_RAM_ATTR Contatore(){  //interruupt del contatore
  PMFContatore++;
}


void setup() { 
  EEPROM.begin(512);
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);

  pinMode(13, INPUT_PULLUP); //Interrupt del flussimetro
  pinMode(PRosso, INPUT_PULLUP); //Pulsante Rosso
  pinMode(PGiallo, INPUT_PULLUP); //Pulsante Giallo
    //pinMode(LED_BUILTIN, OUTPUT);
    //attachInterrupt(digitalPinToInterrupt(13), Contatore, RISING);
  Serial.begin(9600);
    //Wire.begin();
/*  lcd.begin(20,4);        // 20 columns by 4 rows on display
  lcd.setBacklight(HIGH); // Turn on backlight, LOW for off
  lcd.print(F("Ciao Mario"));
  lcd.setCursor (0,1);
  lcd.print(F("PMF Versione 2021_03_21"));
  lcd.setCursor (0,3);
  if (!digitalRead(PGiallo)){
    WiFiEnabled=true;
    lcd.print(F("Wifi Attivata"));  
  }else{
    lcd.print(F("Buon Lavoro!"));        
  }
  lcd.setCursor (0,2);
  lcd.print(PRosso);
  lcd.print(F("-")); 
  lcd.print(PGiallo);

  delay(4000); // Ritardo per la visualizzazione dei dati
  //lcd.setCursor (0,0);
  //lcd.print(F("                    "));
  lcd.setCursor (0,0);
  lcd.print(F("Attendi             "));
  lcd.setCursor (0,1);
  lcd.print(F("Tot:                "));
  lcd.setCursor (0,2);
  lcd.print(F("Flow [cc/mt] :      "));
  lcd.setCursor (0,3);
  lcd.print(F("                    ")); 
*/
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
          //***************************************TCova
          io1=NetCMDS.indexOf("/TCOVA?");
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
            //TCova=S3.toFloat();
            //EEPW(0,(TCova*10));
          } // ******************************************************************

          //***************************************HCova
          io1=NetCMDS.indexOf("/HCOVA?");
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
            //HCova=S3.toFloat();
            //EEPW(5,(HCova*10));
          } // ******************************************************************
          
          //***************************************dTCova
          io1=NetCMDS.indexOf("/DTCOVA?");
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
            //dTCova=S3.toFloat();
            //EEPW(10,(dTCova*10));
          } // ******************************************************************

          //***************************************dHCova
          io1=NetCMDS.indexOf("/DHCOVA?");
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
            //dHCova=S3.toFloat();
            //EEPW(15,(dHCova*10));
          } // ******************************************************************

          //***************************************TimeHR1
          io1=NetCMDS.indexOf("/TIMEHR1?");
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
            //TimeHR1=(S3.toInt())*1000;
            //EEPW(20,(TimeHR1/1000));

          } // ******************************************************************

          //***************************************TimeHR2
          io1=NetCMDS.indexOf("/TIMEHR2?");
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
            //TimeHR2=(S3.toInt())*1000;
            //EEPW(25,(TimeHR2/1000));
          } // ******************************************************************


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
              //client.println(F("Refresh: 5"));
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
          client.println(F("</th></tr>"));
          client.print(F("<tr> <th width=50% align=""right""> Umidita':</th><th width=50% align=""left"">"));
          //client.print(String(Hum,1));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
    
    
    
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Tempo lettura sensore ms</th><th width=50% align=""left"">"));
          //client.print(ReadTempTime);
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



  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************








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

void LogCaldaia(String LogF, String TT){
  WiFiClient WcGMA;
  if (WcGMA.connect("www.mdmecco.it", 80)) {
    WcGMA.print(LogF);
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



word EEPR(byte Id){
  byte Eby1=0;
  byte Eby2=0;
  unsigned int Ev=0;
  Eby1=EEPROM.read(Id);
  Eby2=EEPROM.read(Id+1);
  Ev=word(Eby2,Eby1);
  return Ev;  
}

void EEPW(byte Id, word EvV){
  byte Eby1=0;
  byte Eby2=0;
  Eby1=lowByte(EvV);
  Eby2=highByte(EvV);
  EEPROM.write(Id,Eby1);
  EEPROM.write(Id+1,Eby2);
  EEPROM.commit();
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
