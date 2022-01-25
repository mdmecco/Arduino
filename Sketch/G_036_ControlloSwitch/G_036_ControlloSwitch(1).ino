#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP_EEPROM.h>
//#include "A:\LibMie\MODBUS_CRC.h"
//#include <Wire.h>
#include "DHT.h"

#include "a:\libmie\mecco1.h"


bool Snd = false;
unsigned long Tom1=0;

struct Ser485b {
  unsigned long Time=0;
  char SString[50];
  char RString[50];
  byte MAS=0;
};

struct Ser485b Ser485;

/*

ESP TX == DI convertitore
ESP RX == RO convertitore

Convertitore RE = HIGH 
per uscire con la seriale verso la 485, il pin RE del trasmettitore deve essere HIGH, 













*/


/*
 * 
 * Programma per far funzionare una incubatrice a temperatura ed umidità controllati
 * 
 * 
 * T:37.7   (meglio qualche decimo di meno)
 * 
 * H:45..50 % i primi giorni, poi 65..70% gli ultimi 3 giorni.
 * 
 * Diventa quindi importante il conteggio anche dei giorni.
 * 
 * INPUT:
 * 
 * T 
 * H
 * 
 * OUTPUT:
 * 
 * RISCALDATORE
 * POMPA Umidificatore
 * ROTAZIONE UOVA
 * 
 * Il riscaldatore mantiene la temperatura all'interno della camera alla temperatura prefissata (scarto di 2 decimi di grado da quella misurata), il riscaldatore deve essere
 * all'interno della camera di incubazione per non raffreddarsi e generare degli sbalzi di temperatura.
 * 
 * L'umidità viene generata dal gorgogliare dell'acqua e il riscaldatore ancora non ho definito come usarlo.
 * 
 * 
 * 
 */


//Pin di uscita
#define RISC 14
#define ARIA 12
#define ROT 4


unsigned long DDay=0; //conteggio giorni di cova
float TCova=37.3;     // temperatura di cova
float HCova=60.0;     // umidità di cova
float dTCova=0.2;     // delta regolazione cova
float dHCova=3;                 // delta regolazione umidità
unsigned long TimeHR1=600000;   // tempo attivazione riscaldatore umidità 1
unsigned long TimeHR2=800000;   // tempo attivazione riscaldatore umidità 2
unsigned long TimeRo=7200000;   // periodo attivazione di rotazione
unsigned long TimeRa=10000;      // tempo attivazione motorino di rotazione
unsigned long TimeRoB=0;        // tempo attivazione rotazione
unsigned long TimeRaB=0;        // tempo attivazione durata motorino


float Tem=0;
float Hum=0;
unsigned long RefreshTemp=0;
unsigned long ReadTempTime=0;
unsigned long ReadTempTime0=0;
unsigned long ReadTempTime1=0;
float TTT=0;



#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

#define WEBTITPAGE "Switch_36"
#define PRGVER "2021-08-29 V1.0"
#define LogFile1 "GET /ghelfa/log.php?FileTS=Incubator&StatoR="
#define LogFile2 "GET /ghelfa/log.php?FileTS=Incubator&StatoR="
#define MySIp 36

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




void setup() { 
  EEPROM.begin(512);
  
  //pinMode(RISC, OUTPUT);
  //pinMode(ARIA, OUTPUT);
  //pinMode(ARIAR1, OUTPUT);
  //pinMode(ARIAR2, OUTPUT);
  //pinMode(ROT1, OUTPUT);
  //pinMode(ROT2, OUTPUT);
  //pinMode(0, OUTPUT);

  TCova=(float)EEPR(0)/10;    // temperatura di cova
  HCova=(float)EEPR(5)/10;    // umidità di cova
  dTCova=(float)EEPR(10)/10;   // delta regolazione cova
  dHCova=(float) EEPR(15)/10;   // delta regolazione umidità
  TimeHR1=EEPR(20)*1000; // tempo attivazione riscaldatore umidità 1
  TimeHR2=EEPR(25)*1000; // tempo attivazione riscaldatore umidità 2
  TimeRo=EEPR(30)*1000; // tempo movimenta
  TimeRa=EEPR(35)*1000; // tempo movimenta
  
  //dht.begin();
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);

  Serial.begin(9600);
  pinMode (5 , OUTPUT);
  pinMode (4 , OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  SetUpChannels();
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
            TCova=S3.toFloat();
            EEPW(0,(TCova*10));
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
            HCova=S3.toFloat();
            EEPW(5,(HCova*10));
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
            dTCova=S3.toFloat();
            EEPW(10,(dTCova*10));
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
            dHCova=S3.toFloat();
            EEPW(15,(dHCova*10));
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
            TimeHR1=(S3.toInt())*1000;
            EEPW(20,(TimeHR1/1000));

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
            TimeHR2=(S3.toInt())*1000;
            EEPW(25,(TimeHR2/1000));
          } // ******************************************************************


          //***************************************TimeRO  Periodo rotazione uova
          io1=NetCMDS.indexOf("/TIMERO?");
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
            TimeRo=(S3.toInt())*60000;
            EEPW(25,(TimeRo));
            //comando="";
          } // ******************************************************************


          //***************************************TimeRA  Periodo rotazione uova
          io1=NetCMDS.indexOf("/TIMERA?");
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
            TimeRa=(S3.toInt())*1000;
            EEPW(25,(TimeRo));
            Snd=true;

            


            
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
              //client.println(F("Refresh: 0;url=/"));  // reset the pages
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
          client.print(DayDate);
          client.print(" - " );
          client.print(STime(DaySec()));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
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
    
    
          //************************************************ input TCOVA
          client.print(F("<form action=""TCOVA"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TCOVA-"">Temperatura Incubatrice:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TCOVA-"" name=""-TCOVA-"" value="""));
          client.print(String(TCova,1));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************
    
          //************************************************ input HCOVA
          client.print(F("<form action=""HCOVA"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-HCOVA-"">Umidita' Incubatrice:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-HCOVA-"" name=""-HCOVA-"" value="""));
          client.print(String(HCova,1));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************

          //************************************************ input DTCOVA
          client.print(F("<form action=""DTCOVA"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-DTCOVA-"">delta regolazione Temperatura Incubatrice:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-DTCOVA-"" name=""-DTCOVA-"" value="""));
          client.print(String(dTCova,1));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************
 
          
          //************************************************ input DHCOVA
          client.print(F("<form action=""DHCOVA"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-DHCOVA-"">Delta Regolazione Umidita' Incubatrice:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-DHCOVA-"" name=""-DHCOVA-"" value="""));
          client.print(String(dHCova,1));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************

          //************************************************ input TIMEHR1
          client.print(F("<form action=""TIMEHR1"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TIMEHR1-"">Secondi partenza riscaldatore umidita' 1:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TIMEHR1-"" name=""-TIMEHR1-"" value="""));
          client.print(String((TimeHR1/1000)));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************

          //************************************************ input TIMEHR2
          client.print(F("<form action=""TIMEHR2"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TIMEHR2-"">Secondi partenza riscaldatore umidita' 2:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TIMEHR2-"" name=""-TIMEHR2-"" value="""));
          client.print(String((TimeHR2/1000)));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************
    

          
          //************************************************ input TIMERO  periodo di rotazione uova
          client.print(F("<form action=""TIMERO"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TIMERO-"">Minuti rotazione uova:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TIMERO-"" name=""-TIMERO-"" value="""));
          client.print(String((TimeRo/60000)));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************
          

          //************************************************ input TIMERA  Secondi attivazione motore rotazione
          client.print(F("<form action=""TIMERA"">\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""><label for=""-TIMERA-"">Secondi ti attivazione motore rotazione:</label></th>\r\n"));
          client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TIMERA-"" name=""-TIMERA-"" value="""));
          client.print(String((TimeRa/1000)));
          client.print(F(""" ></th></tr></table><br>\r\n"));
          client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
          client.print(F("</form>\r\n")); 
          //**********************************************************

          
          client.println(F("<hr width=100% size=4 color=FF0000>"));
          client.println(F("<table style=""width:100%"" border=1>"));
          client.print(F("<tr> <th width=50% align=""right""> Tempo lettura sensore ms</th><th width=50% align=""left"">"));
          client.print(ReadTempTime);
          /*
          client.print(" ---");
          MBo[0]=0x01;
          MBo[1]=0x06;
          MBo[2]=0x00;
          MBo[3]=0x01;
          MBo[4]=0x03;
          MBo[5]=0x00;
          NrB=5;
          MBCRC=CRC16(MBo,NrB);
          MBo[6]=highByte(MBCRC);
          MBo[7]=lowByte(MBCRC);
          //comando=PrintArrayHEX(MBo, 7);
          client.print(MBCRC, HEX);
          //client.print(comando);
          */
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


  //InOutManager();

  if (millis() > Tom1){
    Tom1=millis()+1500;
    //InputStatus(1); 
    InOutManager();
  }



/*    
    if (millis() > DayTimeR) {
      GetTime();
      GetDate();
    }
  
/*
struct Ser485 
  unsigned long Time=0;
  char SString[50];
  char RString[50];
  byte MAS=0
*/

/*
  switch(Ser485.MAS){
    case 0:
      if (Snd) {
        Ser485.MAS=5;
        Ser485.Time=millis()+40;  
        digitalWrite(5, HIGH);
      }else{
        Ser485.MAS=100;  
      }
      break;
    case 5:
      if (millis() > Ser485.Time){
        Ser485.Time=millis()+40;
        digitalWrite(4, HIGH);
        Ser485.MAS=10;
      }
      break;
    case 10:
      if (millis() > Ser485.Time){
        Ser485.MAS=20;  
      }  
      break;
    case 20:
      Serial.write(0x01);
      Serial.write(0x06);
      Serial.write(0x00);
      Serial.write(0x01);
      Serial.write(0x03);
      Serial.write(0x00);
      Serial.write(0xD8);
      Serial.write(0xFA);
      Ser485.Time=millis()+40;
      Ser485.MAS=30;  
      break;
    case 30:
      if (millis() > Ser485.Time){
        Ser485.Time=millis()+40;
        digitalWrite(4, LOW);
        Ser485.MAS=40;  
      }  
      break;
    case 40:
      if (millis() > Ser485.Time){
        digitalWrite(5, LOW);
        Ser485.MAS=41;  
      }  
      break;
    case 41:      
      Ser485.MAS=0;  
      Snd=false;
      break;
    case 100:
      if(Serial.available()) {
        comando="";
        Ser485.MAS=120;
      }else{
        Ser485.MAS=0;
      }
      break;
    case 110:
      if(Serial.available()) {
        Ser485.MAS=120;
      }else{
        Ser485.MAS=0;
      }
      break;
    case 120:
      c = Serial.read();
      comando += c;
      Ser485.Time=millis()+30;
      Ser485.MAS=121;
      break;
    case 121:
      if (millis() > Ser485.Time){
        Ser485.MAS=110;  
      }
      break;
    default:
      Ser485.MAS=0;
      Snd=false;
      break;
  }
  
*/
  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************





/*

    str="";
    if (Serial.available()>0) cmd = Serial.read();//leggi il primo       
    switch (cmd) {      
      case 'h':
      if (Serial.available()>0)  sec = Serial.read();//leggi il secondo
            
      while(true){//Cicla--------------
       if (Serial.available()>0)  tch = Serial.read(); 
        if(tch!=13){//se diverso da ritorno carrello
           str+=char(tch);//accumula nella stringa                                                       
         }else{//se invece è un ritorno carrello
          if(sec=='m'){//se il secondo carattere era una 'm'
            Serial.print("Minimo "); Serial.println(str);  
          }else if(sec=='x'){//se il secondo carattere era una 'x'
            Serial.print("Massimo "); Serial.println(str); 
          }
          break;
         }        
      }//End while -------------------
           
        break;         
     }//fine switch 
     cmd=0;
     delay(10);//non togliere

  */

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
