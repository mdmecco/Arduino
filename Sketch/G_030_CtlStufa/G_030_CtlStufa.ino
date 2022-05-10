#include <ESP_EEPROM.h>
#include <Adafruit_MAX31865.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include "a:\libmie\mecco1.h"
//#include <ESP_sleep.h>

//https://www.az-delivery.de/it/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/entwurf-daten-in-den-flash-speicher-des-esp-ablegen

// Interessante per usare bene la EEPROM
//https://sites.google.com/view/marcorossi/home
/*
 Come memorizzare dati nel modulo
Con il modulo ESP8266 � possibile usare una eeprom virtuale. Bisogna ricordare che � possibile scrivere nella memoria non volatile un numero limitato di volte (centinaia di migliaia). Superato il limite non sar� pi� possibile usare il modulo. 
Per usare la eeprom del modulo ESP bisogna includere il file eeprom.h e usare l'oggetto EEPROM. Non essendo una vera eeprom il modulo, con begin si crea semplicemente un array nella ram che verr� riempito con i valori presenti nella "eeprom". Le operazioni di scrittura e lettura useranno questo array e solo chiamando EEPROM.commit() o EEPROM.end() si avr� il vero e proprio salvataggio dei dati nella "eeprom".
EEPROM.Begin(size) serve all'inizializzazione dell'oggetto. Con questa funzione bisogna indicare il numero di byte che  si vuole memorizzare nella "eeprom". La dimensione deve essere compresa tra 4 e 4096 (usate multipli di 4).
EEPROM.write(indirizzo, valore) serve a scrivere un singolo byte nella memoria.
EEPROM.read(indirizzo) serve a leggere un singolo byte dalla memoria.
EEPROM.put(indirizzo, variabile) serve a scrivere un valore nella memoria. Questo valore occuper� tanti byte quanti ne richiede il tipo della variabile passata. Per esempio un int occuper� quattro byte, un bool ne occuper� uno, un doble otto, e cos� via.
EEPROM.get(indirizzo, valore) serve a leggere un valore dalla memoria precedentemente inserito con put().
EEPROM.commit() salva i dati.

EEPROM.end() salva i dati ed elimina l'array. Dopo questo comando non � pi� possibile usare la eeprom fino a quando non si riusi il comando begin().
*/


const IPAddress staticIP(192, 168, 1, 30);
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
unsigned long DayTimeR = 10000;

float MarA=35.0;
float MarB=30.0;

float MauA=35.0;
float MauB=30.0;

float TMin=0;
float TMax=0;

float Temp=0;

#define ArrayLenght 255
float TmpArray[ArrayLenght]={0};
unsigned long ArrayTime=0;
byte ArrayI=0;
#define ArrayRefresh 600000


String S1;
String S2;
String S3;
bool rp=false;
bool OTAActive=false;

bool PMar=false;
bool PMau=false;
bool BMar=false;
bool BMau=false;

bool StMar=false;
bool StMau=false;

bool HomeMar=true;
bool HomeMau=true;


#define rMar 12
#define rMau 14


unsigned long LifePMar=0;
unsigned long LifePMau=0;

unsigned long TimePMar=0;
unsigned long TimePMau=0;


unsigned long TOffPMar=0;
unsigned long TOffPMau=0;

unsigned long TOnPMar=0;
unsigned long TOnPMau=0;

float TCMax=70;
float TRMin=30;


float MaCTMax=0;
float MaCTMin=0;
byte MaCf=0;
float MaCPerc=80;
float MaCTP=0;


// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0


//Adafruit_MAX31865 max = Adafruit_MAX31865(D0, D1, D2, D3);
Adafruit_MAX31865 max31 = Adafruit_MAX31865(16, 5, 4, 0);


void setup() { 
  pinMode(rMar, OUTPUT);
  pinMode(rMau, OUTPUT);

  digitalWrite(rMau, HIGH);
  digitalWrite(rMar, HIGH);

  EEPROM.begin(512);
  Serial.begin(115200);
  
  max31.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  
  MarA = EEPROM.read(0);
  MarB = EEPROM.read(4);
  MauA = EEPROM.read(8);
  MauB = EEPROM.read(12);
  TMin = EEPROM.read(16);
  TMax = EEPROM.read(20);
  LifePMar = EEPROM.read(24);
  LifePMau = EEPROM.read(28);
  TOffPMar = EEPROM.read(32);
  TOffPMau = EEPROM.read(36);
  TOnPMar = EEPROM.read(40);
  TOnPMau = EEPROM.read(44);
  TCMax = EEPROM.read(48);
  
  OTAActive=false;
  
  WiFi.hostname("G030");

}

void loop() {
  //***************** CONNESSIONE WIFI ******************************************************
  if (WifiMas == 0) {
    WifiMas = 1;
    //digitalWrite(LED_BUILTIN, true);
  } else if (WifiMas == 1) {
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
  } else if (WifiMas == 2) {
    WiFi.disconnect();
    WifiMas = 10;
  } else if (WifiMas == 4) {
    //digitalWrite(LED_BUILTIN, false);
    WifiT0 = millis() + 6000;
    WifiMas = 5;
  } else if (WifiMas == 5) {
    if (WiFi.status() == WL_CONNECTED) {
      WifiMas = 6;
    }
    if (millis() > WifiT0 ) {
      WifiMas = 10;
    }
  } else if (WifiMas == 6) {
    //WiFi.setDNS(DNS);
    server.begin();
    WifiMas = 100;
    //ArduinoOTA.begin();
    //GetTime();
  } else if (WifiMas == 10) {
    WifiT0 = millis() + 5000;
    WifiMas = 11;
  } else if (WifiMas == 11) {
    if (millis() > WifiT0) {
      ArduinoOTA.begin(false);
      server.close();
      WiFi.disconnect();
      WifiMas = 0;
    }
  } else if (WifiMas == 100) {
    if (OTAActive){
      ArduinoOTA.handle();
    }
    if (WiFi.status() != WL_CONNECTED) {
      WifiMas = 10;
    }
  }
  //**********************************************************************************

//********** SERVER ****************************************************************
  if (WifiMas ==100) { // Rete attiva e connessa
    if (NetMas==0){
      client = server.available();
      if ((client) & (client.connected())){ // in ricezione
        NetMas=5;
        NetTo=millis()+3000;
      }
    }else if(NetMas==5){
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
    }else if(NetMas==10){
      NetCMDS=client.readStringUntil('-');//
      NetPARS=client.readStringUntil('>');//
      client.print("<OK-00>");
           
      if (millis()>NetTo){
        NetMas=200; // funzione del timeout di ricezione
      }
      NetMas=20;
      
    }else if(NetMas==20){
      NetMas=30;
      if (NetCMDS=="OFFMARIO"){
        HomeMar=false;
      }else if (NetCMDS=="ONMARIO"){
        HomeMar=true;
      }else if (NetCMDS=="OFFMAURIZIO"){
        HomeMau=false;
      }else if (NetCMDS=="ONMAURIZIO"){
        HomeMau=true;
      }
      
    }else if(NetMas==30){
      client.stop();
      NetMas=0; 
      
    }else if(NetMas==50){
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
          UpdateParameter(S2, S3);
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
        TCMax=S3.toFloat();
        EEPROM.write(48,TCMax);
        EEPROM.commit();
      }

//*****************************************  OTA  attivazione web  *****************************      
      io1=NetCMDS.indexOf("/OTA");
      io2=0;
      if (io1 > 0){
        rp=true;
        OTAActive=!OTAActive;
        ArduinoOTA.begin(OTAActive);
      }
//**********************************************************************************************      

      
      io1=NetCMDS.indexOf("/CLEAR?");
      io2=0;
      if (io1 > 0){
        rp=true;
        Temp=max31.temperature(RNOMINAL, RREF);
        TMax=Temp;
        TMin=Temp;
      }
      
      io1=NetCMDS.indexOf("/CLEARSTAT?");
      io2=0;
      if (io1 > 0){
        rp=true;
        LifePMar=0;
        LifePMau=0;
        TimePMar=millis();
        TimePMau=millis();
      }

      io1=NetCMDS.indexOf("/SPEGNIMARIO?");
      io2=0;
      if (io1 > 0){
        rp=true;
        StMar=!StMar;
      }
      
      io1=NetCMDS.indexOf("/SPEGNIMAURIZIO?");
      io2=0;
      if (io1 > 0){
               rp=true;
        StMau=!StMau;
      }

      io1=NetCMDS.indexOf("\FORCELOG");
      io2=0;
      if (io1 > 0){
        rp=true;
        ArrayTime=0;
      }


     while (client.available()){
        char c = client.read();
      }

      //********************************************* pagina server inizio  *****************************************************
      client.println(F("HTTP/1.1 200 OK"));
      client.println(F("Content-Type: text/html"));
      client.println(F("Connection: close"));  // the connection will be closed after completion of the response
      if (rp){
        client.println(F("Refresh: 0;url=/"));  // reset the pages
        rp=false;
      }else{
        //client.println(F("Refresh: 5"));
      }
      client.println();
      client.println(F("<!DOCTYPE HTML>"));
      client.println(F("<html><head><title>Caldaia Viadrus</title></head>"));
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
      int HH=DaySec() / 3600;
      int MM=(DaySec() % 3600)/60;
      //client.print(F("<p align=""center"">"));
      client.print(String(HH));
      client.print(F(":"));
      client.print(String(MM));
      //client.println(F("</p>"));
      client.println(F("</th>"));
      client.print(F("</tr></table>"));
      //************************************************* Fine Header ******************************************************

      
      // *********************************************************************pagina personalizzata *************************
      client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
      client.print(F("<form action=""SET"">\r\n"));
      client.print(F("<table style=""width:100%""  border=1>"));
      client.print(F("<tr> <th width=50% align=""right""><label for=""-TMarON-"">Temperatura accensione Pompa Mario:</label></th>\r\n"));
      client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TMarON-"" name=""-TMarON-"" value="""));
      client.print(String(MarA,0));
      client.print(F(""" ></th></tr>\r\n"));
      
      client.print(F("<tr> <th width=50% align=""right""><label for=""-TMarOFF-"">Temperatura spegnimento Pompa Mario:</label></th>\r\n"));
      client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TMarOFF-"" name=""-TMarOFF-"" value="""));
      client.print(String(MarB,0));
      client.print(F(""" ></th></tr>\r\n"));

      client.print(F("<tr> <th width=50% align=""right""><label for=""-TMauON-"">Temperatura accensione Pompa Maurizio:</label></th>\r\n"));
      client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TMauON-"" name=""-TMauON-"" value="""));
      client.print(String(MauA,0));
      client.print(F(""" ></th></tr>\r\n"));
      
      client.print(F("<tr> <th width=50% align=""right""><label for=""-TMauOFF-"">Temperatura spegnimento Pompa Maurizio:</label></th>\r\n"));
      client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TMauOFF-"" name=""-TMauOFF-"" value="""));
      client.print(String(MauB,0));
      client.print(F(""" ></th></tr></table><br>\r\n"));
      
      client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
      client.print(F("</form>\r\n")); 
      client.print(F("<hr width=100% size=4 color=FF0000>"));
      client.print(F("<table style=""width:100%"" border=1>"));
      client.print(F("<tr><th width=50% align=""right"">Temperatura Caldaia</th><th width=50% align=""left"">"));
      client.print(String(Temp,1));
      client.println(F("</th></tr>"));

      
      client.print(F("<tr> <th width=50% align=""right"">Temperatura Max Riscaldamento Sanitario:</th>\r\n"));
      client.print(F(" <th width=50% align=""left"">"));
      client.print(String(MaCTMax,1));
      client.println(F("</th></tr>"));

      client.print(F("<tr> <th width=50% align=""right"">Temperatura Min Riscaldamento Sanitario:</th>\r\n"));
      client.print(F(" <th width=50% align=""left"">"));
      client.print(String(MaCTMin,1));
      client.println(F("</th></tr>"));





      
      client.print(F("<tr><th width=50% align=""right"">Pompa riscaldamento Mario</th>"));
      if (PMar){
        client.println(F("<th width=50% align=""left"">ACCESA</th>"));
      }else{
        client.println(F("<th width=50% align=""left"">SPENTA</th></tr>"));
      }
      client.print(F("<tr><th width=50% align=""right"">Pompa riscaldamento Maurizio</th>"));
      if (PMau){
        client.println(F("<th width=50% align=""left"">ACCESA</th>"));
      }else{
        client.println(F("<th width=50% align=""left"">SPENTA</th>"));
      }

      client.println(F("</tr></table>"));
      
      client.println(F("<hr width=100% size=4 color=FF0000>"));
      client.println(F("<table style=""width:100%"" border=1>"));
      client.print(F("<tr> <th width=50% align=""right""> TMax:</th><th width=50% align=""left"">"));
      client.print(String(TMax,1));
      client.println(F("</th></tr>"));
      client.print(F("<tr> <th width=50% align=""right""> TMin:</th><th width=50% align=""left"">"));
      client.print(String(TMin,1));
      client.println(F("</th>"));
      client.print(F("</tr></table>"));

      client.print(F("<form action=""CLEAR"">\r\n"));
      client.print(F("<input type=""submit"" value=""Reset MAX-MIN"" name=""RMAXMIN"">\r\n"));
      client.print(F("</form>\r\n")); 
      client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
      
      
      client.print(F("<form action=""TCMAX"">\r\n"));
      client.print(F("<table style=""width:100%"" border=1>"));
      client.print(F("<tr> <th width=50% align=""right""><label for=""-TCMAX-"">Temperatura Massima Caldaia:</label></th>\r\n"));
      client.print(F(" <th width=50% align=""left""><input type=""text"" id=""-TCMAX-"" name=""-TCMAX-"" value="""));
      client.print(String(TCMax,0));
      client.print(F(""" ></th></tr></table><br>\r\n"));
      client.print(F("<input type=""submit"" value=""Submit"">\r\n"));
      client.print(F("</form>\r\n")); 



      client.println(F("<hr width=100% size=4 color=FF0000>"));
      
      client.println(F("<table style=""width:100%""  border=1>"));
      client.print(F("<tr><th width=50% align=""right"">Tempo Attività Mario:</th><th width=50% align=""left"">"));
      if (PMar){
        client.print(String((millis()-TimePMar)/1000));
      }else{
        client.print("0");  
      }
      client.println(F("</th></tr>"));
      client.print(F("<tr> <th width=50% align=""right"">Tempo Attività Maurizio:</th><th width=50% align=""left"">"));
      if (PMau){
        client.print(String((millis()-TimePMau)/1000));
      }else{
        client.print("0");  
      }
      client.println(F("</th></tr>"));
      
      client.print(F("<tr><th width=50% align=""right"">Tempo Totale Attività Mario:</th><th width=50% align=""left"">"));
      client.print(String(LifePMar/60));
      client.println(F("</th></tr>"));
      client.print(F("<tr> <th width=50% align=""right"">Tempo Attività Maurizio:</th><th width=50% align=""left"">"));
      client.print(String(LifePMau/60));
      client.println(F("</th></tr>"));
      client.print(F("</table>"));

      client.print(F("<form action=""CLEARSTAT"">\r\n"));
      client.print(F("<input type=""submit"" value=""Clear Stats"" name=""CCLEARSTATS"">\r\n"));
      client.print(F("</form>\r\n")); 
      client.println(F("<hr width=100% size=4 color=FF0000>"));

      client.println(F("<table style=""width:100%""  border=1>"));
      client.print(F("<tr><th width=50% align=""right"">Orario ultima accensione Impianto Mario:</th><th width=50% align=""left"">"));
      client.print(STime(DaySec()));
      client.println(F("</th></tr>"));
      client.print(F("<tr> <th width=50% align=""right"">Orario ultima accensione Impianto Maurizio:</th><th width=50% align=""left"">"));
      HH=TOnPMau / 3600;
      MM=(TOnPMau % 3600)/60;
      client.print(String(HH));
      client.print(F(":"));
      client.print(String(MM));
      client.println(F("</th></tr>"));

      client.print(F("<tr><th width=50% align=""right"">Orario ultimo spegnimento Impianto Mario:</th><th width=50% align=""left"">"));
      HH=TOffPMar / 3600;
      MM=(TOffPMar % 3600)/60;
      client.print(String(HH));
      client.print(F(":"));
      client.print(String(MM));
      client.println(F("</th></tr>"));
      client.print(F("<tr> <th width=50% align=""right"">Orario ultimo spegnimento Impianto Maurizio:</th><th width=50% align=""left"">"));
      HH=TOffPMau / 3600;
      MM=(TOffPMau % 3600)/60;
      client.print(String(HH));
      client.print(F(":"));
      client.print(String(MM));
      client.println(F("</th></tr>"));
      client.print(F("</table>"));

      client.println(F("<hr width=100% size=4 color=FF0000>"));


      client.print(F("<form action=""SPEGNIMARIO"">\r\n"));
      client.print(F("<input align=""center"" type=""submit"" value=""Spegni_Pompa_Mario"" name=""SPEGNIMARIO"">\r\n"));
      client.print(F("</form>\r\n")); 
      client.println(F("<hr width=100% size=4 color=FF0000>"));

      client.print(F("<form action=""SPEGNIMAURIZIO"">\r\n"));
      client.print(F("<input align=""center"" type=""submit"" value=""Spegni_Pompa_Maurizio"" name=""SPEGNIMAURIZIO"">\r\n"));
      client.print(F("</form>\r\n")); 
      client.println(F("<hr width=100% size=4 color=FF0000>"));
      
      client.println(F("<table style=""width:100%""  border=1>"));
      client.println(F("<tr><th width=50% align=""right"">Temperatura di reset comandi da casa:</th>"));
      client.println(F("<th width=50% align=""left""> <input name=""TMaxReset"" value=""30"" min=""0"" max=""90"" autocomplete=""off"">\r\n </th></tr></table>"));

      client.println(F("<hr width=100% size=4 color=FFFF00>"));

      client.println(F("<p align=""center""><a href=""\FORCELOG"">Forza il log dello stato attuale</a></p>"));

      
      client.println(F("\r\n</body>\r\n</html>"));
      delay(100);
      NetMas=0; // funzione del timeout di ricezione
      client.stop();
    }else if(NetMas==200){
      client.stop();
      NetMas=0;
    }
//***********************************************************************
//*********************** codice dentro rete  ***************************
//***********************************************************************
    if (millis() > DayTimeR) {
      GetTime();
    }

  if ( millis() > ArrayTime ){
    ArrayTime=millis()+ArrayRefresh;
    TmpArray[ArrayI]=Temp;
    ArrayI = ArrayI + 1;

    S1=String(Temp,1);
    S1.trim();
    S1.concat(';');
    if (PMar){
      S1.concat('1');  
    }else{
      S1.concat('0');
    }

    S1.concat(';');
    if (PMau){
      S1.concat('1');  
    }else{
      S1.concat('0');
    }

    LogCaldaia(S1);
    S1="";
  }




//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************

  Temp=max31.temperature(RNOMINAL, RREF);

  if (Temp > MarA){
    PMar=true;
  }
  if (Temp < MarB){
    PMar=false;
    StMar=false;
  }
  if (Temp > MauA){
    PMau=true;
  }
  if (Temp < MauB){
    PMau=false;
    StMau=false;
  }

  // controllo attivazione dalla temperatura di casa
  PMau= PMau && HomeMau;
  PMar= PMar && HomeMar;
 //********************************************************
  

// questa parte serve per bloccare la pompa quando la temperatura è abbastanza alta da tenere l'acqua sanitaria calda (solo per Mario)
 if (Temp > MaCTMax){
  MaCTMax=Temp;
  MaCf=0;
 }

if (MaCf==0){
  MaCTMin=MaCTMax * MaCPerc / 100 ;
  if (Temp > MaCTMin) {
    StMar=false;
  }else{
    StMar=true;
    MaCf=1;
  }
}

  
  
  
  PMau= PMau && !StMau;
  PMar= PMar && !StMar;

  
// nel caso si scenda sotto la temperatura minima si resetta tutto  
  if (Temp < TRMin){
    HomeMau=true;
    HomeMar=true;
    PMar=false;
    PMau=false;
    MaCTMax=0;
  }
  
   
  // questa parte serve per la sicurezza che se la temperatura è troppo elevata, le pompe girano senza blocchi
  if (Temp > TCMax){
    PMar=true;
    PMau=true;
    StMar=false;
    StMau=false;
  }


// Settaggio della pompa MARIO
  if (PMar != BMar){  
    if ((PMar)&&(!StMar)) { 
      TimePMar=millis();
      TOnPMar=DaySec();
      EEPROM.write(40,TOnPMar);
      EEPROM.commit();
    }else{
      LifePMar=LifePMar + ((millis()-TimePMar)/1000);
      TOffPMar=DaySec();
      EEPROM.write(32,TOffPMar);
      EEPROM.write(24,LifePMar);
      EEPROM.commit();
    }
    BMar=PMar;
    digitalWrite(rMar, !PMar);
  }

  
// Settaggio della pompa MAURIZIO
  if (PMau != BMau){  
    if ((PMau)&&(!StMau)) {
      TimePMau=millis();
      TOnPMau=DaySec();
      EEPROM.write(44,TOnPMau);
      EEPROM.commit();
    }else{
      LifePMau=LifePMau + ((millis()-TimePMau)/1000);
      TOffPMau=DaySec();
      EEPROM.write(36,TOffPMau);
      EEPROM.write(28,LifePMau);
      EEPROM.commit();
    }
    BMau=PMau;
    digitalWrite(rMau, !PMau);
  }

  if (Temp > TMax){
    TMax=Temp;
    EEPROM.write(20,TMax);
    EEPROM.commit();
  }
  
  if (Temp < TMin){
    TMin=Temp;
    EEPROM.write(16,TMin);
    EEPROM.commit();
  }


//**************************************************************************************
}
//********** fine MAIN  ***************************************************************
//**************************************************************************************
//------------------------------------------------- Funzioni --------------------------

void UpdateParameter(String SS, String PP){
  if (SS=="-TMarON-"){
    MarA=PP.toFloat();
    EEPROM.write(0,MarA);
    //Serial.println(MarA);
  }
  if (SS=="-TMarOFF-"){
    MarB=PP.toFloat();
    EEPROM.write(4,MarB);
    //Serial.println(MarB);
  }
  if (SS=="-TMauON-"){
    MauA=PP.toFloat();
    EEPROM.write(8,MauA);
    //Serial.println(MauA);
  }
  if (SS=="-TMauOFF-"){
    MauB=PP.toFloat();
    EEPROM.write(12,MauB);
    //Serial.println(MauB);
  }
  EEPROM.commit();
}



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
        //Serial.print(cll);
      }
    }
    //delay(100);
    WcGMA.flush();
    WcGMA.stop();
    //Serial.println("--STOP--");
  }
}

void LogCaldaia(String TT){
  //  http://www.mdmecco.it/ghelfa/log.php?FileTS=FINO&StatoR=100.54
  if (client.connect("www.mdmecco.it", 80)) {
    //client.print(F("GET /ghelfa/time.php HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    client.print(F("GET /ghelfa/log.php?FileTS=Caldaia&StatoR="));
    client.print(TT);
    client.print(F(" HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    client.flush();
  }
  client.stop();
}


void GetTime() {
  String Ln1 = "";
  unsigned long TT = 0;
  WiFiClient GTime; 
  if (GTime.connect("www.mdmecco.it", 80)) {
    GTime.print(F("GET /ghelfa/time.php HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    GTime.flush();
    //delay(150);
    Ln1 = GTime.readStringUntil('<');
    Ln1 = GTime.readStringUntil('>');
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
  GTime.stop();
  //GTime.close();
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
