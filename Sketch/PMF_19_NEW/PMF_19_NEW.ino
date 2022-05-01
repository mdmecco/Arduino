d#include "A:\libraries\AAMecco\pulsanti.h"

#include <ArduinoOTA.h>
#include "A:\libraries\ESP8266WiFi\src\ESP8266WiFi.h"
//#include <LiquidCrystal_I2C.h>
//#include <TinyGPS++.h>
#include <Wire.h>

#include "A:\libraries\New-LiquidCrystal-master\LiquidCrystal_I2C.h"
#include "A:\libraries\TinyGPS\TinyGPS++.h"

//#include "A:\libraries\LiquidCrystal_I2C\LiquidCrystal_I2C.h"


TinyGPSPlus gps;

static const uint32_t GPSBaud = 9600;
const IPAddress staticIP(192,168,1,19);


const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";
const char* ssid3 = "DGhelfa";
const char* password3 = "Lamborghini";

IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
IPAddress E1(192,168,1,10);
WiFiServer server(80);
WiFiClient clientb; 
WiFiClient client;
WiFiClient Climdmecco;
WiFiClient ESPM;
byte WifiMas=0;  //Macchina a stati per la connessione WIFI
unsigned long WifiT0=0; // tempi di attesa per la connessione wifi
unsigned long WifiT1=0;

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
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);


unsigned long DelayDateDisplay=0;
unsigned int WId=0;


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

byte WiFiClMAS=0;
unsigned long WiFiClTo=0;
bool WiFiBo=false;
String WiFiSt="";
char WiFiCh=0;

bool WiFiEnabled=false;

void ICACHE_RAM_ATTR Contatore(){  //interruupt del contatore
  PMFContatore++;
}

void setup() {
    pinMode(13, INPUT_PULLUP); //Interrupt del flussimetro
    pinMode(PRosso, INPUT_PULLUP); //Pulsante Rosso
    pinMode(PGiallo, INPUT_PULLUP); //Pulsante Giallo
    //pinMode(LED_BUILTIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(13), Contatore, RISING);
    Serial.begin(9600);
    Wire.begin();
   lcd.begin(20,4);        // 20 columns by 4 rows on display
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

}

void loop() {
  
  if (Serial.available() > 0){        //Lettura del GPS
    gps.encode(Serial.read());
  }

//***************** CONNESSIONE WIFI ******************************************************
  if (WiFiEnabled){
    if (WifiMas==0) {
      lcd.setCursor (0,3);
      lcd.print(F("Scanning WiFi...    "));
      WifiMas=1;
      //digitalWrite(LED_BUILTIN, true);
    }else if (WifiMas ==1){
      if (WIFIScan()>0) {
        WiFi.config(staticIP, gateway, subnet);
        WiFi.hostname("PFM_19");
        lcd.setCursor (0,3);   
        if (WId==1){
          WiFi.begin(ssid1, password1);
          lcd.print(F("GGhelfa             "));
          WifiMas=4; 
        }else if (WId==2){
          WiFi.begin(ssid2, password2);
          lcd.print(F("WGhelfa             "));
          WifiMas=4;
        }else if (WId==3){
          WiFi.begin(ssid3, password3);
          lcd.print(F("DGhelfa             "));
          WifiMas=4;    
        }else{
          WifiMas=2;
        }     
      }
    }else if (WifiMas ==2){
      WiFi.disconnect();
      WifiMas=10;
    }else if (WifiMas ==4){
      //digitalWrite(LED_BUILTIN, false);
      WifiT0=millis()+6000;
      WifiMas=5;
    }else if (WifiMas ==5){  
      if (WiFi.status() == WL_CONNECTED){
        WifiMas=6;
      }
      if (millis()> WifiT0 ) {
        WifiMas=10;
      }
    }else if (WifiMas ==6){  
      server.begin();
      client = server.available();
      WifiMas=100;
      lcd.setCursor (0,3);
      lcd.print(F("Connected.          "));
      ArduinoOTA.begin();
      //digitalWrite(LED_BUILTIN, true);
    }else if (WifiMas ==10){
      WifiT0=millis()+5000;
      WifiMas=11;
    }else if (WifiMas ==11){
      if (millis()> WifiT0) {
        ArduinoOTA.begin(false);
        //digitalWrite(LED_BUILTIN, false);
        server.close();
        WiFi.disconnect();
        WifiMas=0;
      }
    }else if (WifiMas ==100){
      ArduinoOTA.handle();
      client = server.available();
      if (WiFi.status() != WL_CONNECTED){
        WifiMas=10;
      }
    }
  }
//**********************************************************************************
  
  if (millis() > DelayDateDisplay){   // refresh del display
    DelayDateDisplay=millis()+ 1000; // ciclo di visualizzazione della data e ora  
    
   //******************************** DATA ******************************************
    if (gps.date.isValid()){
      lcd.setCursor (0,0);
      if (gps.date.day() < 10) lcd.print(F("0"));
      lcd.print(gps.date.day());
      lcd.print(F("/"));
      if (gps.date.month() < 10) lcd.print(F("0"));
      lcd.print(gps.date.month());
      lcd.print(F("/"));
      lcd.print(gps.date.year());
    }
    //********************************   TIME  *****************************************
    if (gps.time.isValid()){
      lcd.setCursor (12,0);
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
    lcd.print(F("                    "));
    lcd.setCursor (0,3);
    if (iMenu==2){
      lcd.print(gps.location.lat(), 6);
      lcd.print(F("  "));
      lcd.print(gps.location.lng(), 6);
    }else if(iMenu==1){
       lcd.print(F("Speed:"));
       lcd.print(gps.speed.kmph(),2);
       lcd.print(F("    "));
    }else if(iMenu==3){
       lcd.print(F("Altitude:"));
       lcd.print(gps.altitude.meters());
       lcd.print(F("    "));
    }else if(iMenu==4){
       lcd.print(F("Nr of Satellite:"));
       lcd.setCursor (16,3);
       lcd.print(gps.satellites.value());
       //lcd.print(FmyS);
    }else if(iMenu==5){
       lcd.print(F("Counter:"));
       lcd.print(PMFContatore);
       lcd.print(F("    "));
    }else if(iMenu==6){
       lcd.print(F("Distance:"));
       lcd.print(Distanza(Plat,Plng,gps.location.lat(),gps.location.lng()),1);
       lcd.print(F("    "));
    }else if(iMenu==7){
       lcd.print(F("Luce Lampione     "));
    }else{
       lcd.print(F("Flow Type"));
    }  
  }

//************************** iMenu  ********** Gestione delle funzioni display e tast **********************
    bitWrite(SPG,0,!digitalRead(PGiallo));
    ProcBtn(SPG,TARG,TOnActP, TATG);
    if (bitRead(SPG,7)==1){
      lcd.setCursor(0,3);
      lcd.print(F("                    "));
      iMenu+=1;
      iMenu = iMenu % 8;
    }

    bitWrite(SPR,0,!digitalRead(PRosso));
    ProcBtn(SPR,TARR,TOnActP, TATR);
    if (bitRead(SPR,7)==1){
      if(iMenu==6){
        Plat=gps.location.lat();
        Plng=gps.location.lng();
      }else if(iMenu==0){
        FlowType =!FlowType;
        lcd.setCursor (0,2); 
        if (FlowType) {
          lcd.print (F("Flow[lt/min]:"));
        }else{
          lcd.print (F("Flow[cc/mt] :"));
        }
      }else{
        lcd.setCursor(0,1);
        lcd.print(F("                    "));
        lcd.setCursor (0,1);
        lcd.print (F("Tot:"));
        PMFContatore=0;
      }
    }

  
  if (millis() > pulse_WaitT){    //Calcoli di misura del flusso
    lcd.setCursor (0,1);
    lcd.print(F("                    "));
    lcd.setCursor (0,1);
    lcd.print(F("Tot:"));
    StTot = String((double)((double) PMFContatore * (double)TrCal),1);
    lcd.setCursor ((20-StTot.length()),1);
    lcd.print(StTot);
    lcd.setCursor (0,2);
    lcd.print(F("                    "));
    lcd.setCursor (0,2);
    if (FlowType) {
      lcd.print (F("Flow[lt/min]:"));
    }else{
      lcd.print (F("Flow[cc/mt] :"));
    }

    if (FlowType) {
      lt_mt=((double)(PMFContatore-pulse_counter2)*((double)60000*(double)TrCal))/(double)(millis()-pulse_counterT); //litri al minuto
    }else{
      lt_mt=((double)(PMFContatore-pulse_counter2)*((double)1000*(double)TrCal))/(double)((double)(millis()-pulse_counterT)*(double)gps.speed.mps())*1000;   //cc al metro
    }
    if (lt_mt<0.01){
      lt_mt=0;
    }
    if (lt_mt>600){
      lt_mt=0;
    }
    StS = String(lt_mt,1);
    lcd.setCursor ((20-StS.length()),2);
    lcd.print(StS);
    
    pulse_WaitT=millis()+MSecCampionamento;
    pulse_counter2=PMFContatore;
    pulse_counterT=millis();
  }


//************************* read server *********************
  if (WiFiEnabled){
    if (client){
      if (client.connected()){
        if (WiFiClMAS==0) {
          if (client.available()){
            WiFiCh = client.read();
            WiFiClTo=millis()+100;
          }
          if (millis() > WiFiClTo) {
            WiFiClMAS=2;  
          }

        }else if (WiFiClMAS==2) {
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          client.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.println();

          client.print(F("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\r\n<html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"><title>Lamborghini 80</title></head>"));
          client.print(F("<body>\r\n<table style=\"text-align: left; width: 100%;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">\r\n<tbody>\r\n<tr>\r\n<td>Totale:</td>\r\n<td>"));
          client.print(StTot);
          client.print(F("</td>\r\n</tr>\r\n<tr>\r\n<td>Posizione:</td><td>"));
          client.print(gps.location.lat());
          client.print(F(" - "));
          client.print(gps.location.lng());
          client.print(F("</td>\r\n</tr>\r\n<tr>\r\n<td>Velocità:</td>\r\n<td>"));
          client.print(gps.speed.kmph(),2);
          client.print(F("</td>\r\n</tr>\r\n<td>Mappa</td>\r\n<td>"));
          client.print(F("<iframe width=\"425\" height=\"350\" frameborder=\"0\" scrolling=\"no\" marginheight=\"0\" marginwidth=\"0\" src=\"https://www.openstreetmap.org/export/embed.html?bbox="));
          client.print(gps.location.lng()-0.003);
          client.print(F("%2C"));
          client.print(gps.location.lat()-0.003);
          client.print(F("%2C"));
          client.print(gps.location.lng()+0.003);
          client.print(F("%2C"));
          client.print(gps.location.lat()+0.003);
          client.print(F("&amp;layer=mapnik\" style=\"border: 1px solid black\"></iframe></td>\r\n</tr>\r\n<td>Distanza</td>\r\n<td>"));
          //<iframe width="425" height="350" frameborder="0" scrolling="no" marginheight="0" marginwidth="0" src="https://www.openstreetmap.org/export/embed.html?bbox=11.09275817871094%2C44.87026420557226%2C11.10097646713257%2C44.874681722610646&amp;layer=mapnik" style="border: 1px solid black"></iframe>
          client.print(F("</td>\r\n</tr>\r\n</tbody>\r\n</table>\r\n</body></html>"));
          WiFiClMAS=2;
          WiFiClTo=millis()+500;
        }else if (WiFiClMAS==2) {
          if (millis() > WiFiClTo) {
            WiFiClMAS=3;  
          }
        }else if (WiFiClMAS==3) {
          client.stop();      
          WiFiClMAS=0;
        }
      }
    }
  }
}
//******************************** Server Close *******************************************

//------------------------------------------------- Funzioni --------------------------
int WIFIScan() {                                                                           //Scan delle Wifi
  int RssV=-1000;
  WId=0;
  WiFi.scanDelete();
  int n = WiFi.scanNetworks();
  if (n == 0){
  }else{
   for (int i = 0; i < n; ++i){
    if (WiFi.SSID(i)=="GGhelfa"){
     if (WiFi.RSSI(i) > RssV){
      RssV=WiFi.RSSI(i);
      WId=1;
     }
    }
    if (WiFi.SSID(i)=="WGhelfa"){
     if (WiFi.RSSI(i)> RssV){
      RssV=WiFi.RSSI(i);
      WId=2;
     }
    }
    if (WiFi.SSID(i)=="DGhelfa"){
     if (WiFi.RSSI(i)> RssV){
      RssV=WiFi.RSSI(i);
      WId=3;
     }
    }
    // delay(10);
   }
  }
  return WId;
}
double Distanza(double lat1, double long1, double lat2, double long2){                     //Misura della distanza tra due coordinate
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = radians(long1-long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
}


void SendButtonM(String StoSend){
  ESPM.connect( E1, 80);
  while (ESPM.connected()){
    ESPM.print(StoSend);  
    delay(50);
    ESPM.stop();
  }
}
