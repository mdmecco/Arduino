#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "a:\libmie\pulsanti.h"
#include "a:\libmie\mecco1.h"


#define WEBTITPAGE "ESP 8 Relè"
#define PRGVER "2022-12-30 V2.0 UDP"
#define MySIp 13

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
char T[]={0,0,0,0,0};

//*********************************** UDP ************************************
WiFiUDP MUdp;
int localUdpPort=5240;
int packetSize=0;
char incomingPacket[256];
//****************************************************************************


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

// Lista Uscite utilizzate

  #define Acceso HIGH
  #define Spento LOW

byte L[9] ={2,5,4,0,15,13,12,14,16};

unsigned long TL[9] ={0,0,0,0,0,0,0,0,0};

//Durate di default in millis della accensioni delle singole uscite
unsigned long DL[9] ={10000,10000,10000,10000,10000,10000,10000,10000,10000};

byte BL[9] ={0,0,0,0,0,0,0,0,0};

void setup() { 
  LittleFS.begin();
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);
  pinMode(L[0], OUTPUT);
  pinMode(L[1], OUTPUT);
  pinMode(L[2], OUTPUT);
  pinMode(L[3], OUTPUT);
  pinMode(L[4], OUTPUT);
  pinMode(L[5], OUTPUT);
  pinMode(L[6], OUTPUT);
  pinMode(L[7], OUTPUT);
  pinMode(L[8], OUTPUT);
  
  digitalWrite(L[0], Spento);
  digitalWrite(L[1], Spento);
  digitalWrite(L[2], Spento);
  digitalWrite(L[3], Spento);
  digitalWrite(L[4], Spento);
  digitalWrite(L[5], Spento);
  digitalWrite(L[6], Spento);
  digitalWrite(L[7], Spento);
  digitalWrite(L[8], Spento);
  
  MUdp.begin(localUdpPort);

  digitalWrite(L[0], Spento);

   if (!loadConfig()){
      DL[1] = 10000;
      DL[2] = 10000;
      DL[3] = 10000;
      DL[4] = 10000;
      DL[5] = 10000;
      DL[6] = 10000;
      DL[7] = 10000;
      DL[8] = 10000;
   }  
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
      digitalWrite(L[0], Spento);
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
      digitalWrite(L[0], Acceso);   // da qui sono collegato con la wifi
      break;
    case 7:
      server.begin(false);
      WifiT0 = millis() + 5000;
      WifiMas = 11;
      break;
    case 11:
      if (millis() > WifiT0) {
        digitalWrite(L[0], Acceso);
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
        if (NetCMDS=="STATUS"){
          client.print("<STATUS-");
          client.print(Status(), DEC);
          client.print(">");
          
        }else if (NetCMDS=="L1"){
          Slr(1,GetValue(NetPARS));
        }else if (NetCMDS=="L2"){
          Slr(2,GetValue(NetPARS));
        }else if (NetCMDS=="L3"){
          Slr(3,GetValue(NetPARS));
        }else if (NetCMDS=="L4"){
          Slr(4,GetValue(NetPARS));
        }else if (NetCMDS=="L5"){
          Slr(5,GetValue(NetPARS));
        }else if (NetCMDS=="L6"){
          Slr(6,GetValue(NetPARS));
        }else if (NetCMDS=="L7"){
          Slr(7,GetValue(NetPARS));
        }else if (NetCMDS=="L8"){
          Slr(8,GetValue(NetPARS));
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
            }
          }

//*************************************** LETTURA VALORE PARAMETRO ***********************************
      io1=NetCMDS.indexOf("NPPD");
      if (io1 > 0){
        StParam();
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


    //*****************************************  Prametri  *****************************      
          io1=NetCMDS.indexOf("/PAR");
          io2=0;
          if (io1 > 0){
            NetMas=150;
            break;
          }
    //**********************************************************************************************      
          
          
          
          
          //***************************************L1
          io1=NetCMDS.indexOf("/L1 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(1,0);
          } // ******************************************************************
          
          //***************************************L2
          io1=NetCMDS.indexOf("/L2 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(2,0);
          } // ******************************************************************


          //***************************************L3
          io1=NetCMDS.indexOf("/L3 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(3,0);
          } // ******************************************************************

          //***************************************L4
          io1=NetCMDS.indexOf("/L4 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(4,0);
          } // ******************************************************************

          //***************************************L5
          io1=NetCMDS.indexOf("/L5 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(5,0);
          } // ******************************************************************

          //***************************************L6
          io1=NetCMDS.indexOf("/L6 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(6,0);
          } // ******************************************************************

          //***************************************L7
          io1=NetCMDS.indexOf("/L7 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(7,0);
          } // ******************************************************************

          //***************************************L8
          io1=NetCMDS.indexOf("/L8 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            Slr(8,0);
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
          client.print("  --- Status:");
          client.print(Status(), BIN);
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

            client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
            
            client.print(F("<form action=""/PAR"">"));
            client.print(F("<input type=""submit"" value=""Parametri"">"));
            client.print(F("</form>")); 
      
            client.print(F("<form action=""/OTA"">"));
            client.print(F("<input type=""submit"" value=""OTA"">"));
            client.print(F("</form>")); 

            client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));


          //**********************************************  L1  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL[1]==5) {
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
            if (BL[2]==5) {
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
            if (BL[3]==5) {
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
            if (BL[4]==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L4\" >___L4___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            


          //**********************************************  L5  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL[5]==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L5\" >___L5___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          //**********************************************  L6  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL[6]==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L6\" >___L6___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            
        
          //**********************************************  L7  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL[7]==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L7\" >___L7___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          //**********************************************  L8  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            if (BL[8]==5) {
              client.println(BtnColor(1));
            }else{
              client.println(BtnColor(0));
            }
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L8\" >___L8___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            
            
            client.println(F("</body>\r\n</html>"));
            delay(100);
            NetMas=0; // funzione del timeout di ricezione
            client.stop();
        }
        break;

      case 150:
        {
          while (client.available()){
            char c = client.read();
          }

          //********************************************   PARAMETRI   *********************************************************    
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.print(F("<html><head><title>"));
          client.print(WEBTITPAGE);
          client.print("  Parametri");
          client.println(F("</title></head>"));
          client.println(F("<body>"));
          client.print(F("<b style=""font-size:10px"">"));
          client.print(PRGVER);
          client.print("  --- Status:");
          client.print(Status(), BIN);
          client.print(F("</b>"));
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
          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
          
          client.print(F("<form action=""/"">"));
          client.print(F("<input type=""submit"" value=""Main"">"));
          client.print(F("</form>")); 
    
          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
          ClParam(1);
          ClParam(2);
          ClParam(3);
          ClParam(4);
          ClParam(5);
          ClParam(6);
          ClParam(7);
          ClParam(8);
          client.println(F("</body>\r\n</html>"));
          delay(100);
          NetMas=200; // funzione del timeout di ricezione
          //client.stop();
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
   
//****************   UDP   **********************************************
    packetSize = MUdp.parsePacket();
    if (packetSize){
      int len = MUdp.read(incomingPacket, 255);
      if (len > 0){
        char a=incomingPacket[0];
        if (a='L') {
          a=incomingPacket[1];
          T[0]=incomingPacket[3];
          T[1]=incomingPacket[4];
          T[2]=incomingPacket[5];
          T[3]=incomingPacket[6];
          T[4]=incomingPacket[7];
          int f=int(a)-48;
          len=atoi(T);
          Slr(f,len);
        }
      }
    }

//************************************************************************

    if (millis() > DayTimeR) {
      GetTime();
      GetDate();
    }



  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************

  Olr(1);
  Olr(2);
  Olr(3);
  Olr(4);
  Olr(5);
  Olr(6);
  Olr(7);
  Olr(8);
 
//**************************************************************************************
}
//********** fine MAIN  ***************************************************************
//**************************************************************************************
//------------------------------------------------- Funzioni --------------------------

void Olr(byte Id){
  if (BL[Id]==5){ //controlla se spegnere
    if ( millis() > TL[Id] ){
      digitalWrite(L[Id], Spento);
      BL[Id]=0;
    }
  }else if (BL[Id]==6){  // Accendi la luce
    digitalWrite(L[Id], Acceso);
    BL[Id]=5;
  }else if (BL[Id]==7){  // Spegni Subito
    digitalWrite(L[Id], Spento);
    BL[Id]=0;
  }  
}

void Slr(byte Id, unsigned long Td){
  if (BL[Id]!=0){
    BL[Id]=7;
  }else{
    if (Td ==0) {
      TL[Id]=DL[Id] + millis();
    }else{
      TL[Id]=(Td*1000) + millis();
    }
    BL[Id]=6;
  }
}

byte Status (){
  byte Stat=0;
  if (BL[1]!=0){
    Stat=Stat+1;
  }
  if (BL[2]!=0){
    Stat=Stat+2;
  }
  if (BL[3]!=0){
    Stat=Stat+4;
  }
  if (BL[4]!=0){
    Stat=Stat+8;
  }
  if (BL[5]!=0){
    Stat=Stat+16;
  }
  if (BL[6]!=0){
    Stat=Stat+32;
  }
  if (BL[7]!=0){
    Stat=Stat+64;
  }
  if (BL[8]!=0){
    Stat=Stat+128;
  }
  return Stat;
}


void ClParam(byte Id){
      client.print(F("<form action=""/PAR"));
      client.print(F(">\r\n"));
      client.print(F("<table style=""width:100%"" border=1>"));
      client.print(F("<tr> <th width=50% align=""right""><label for=""-PD"));
      client.print(Id);
      client.print(F("-"">Durata Accensione in secondi canale:"));
      client.print(Id);
      client.print(F("</label></th>\r\n"));
      client.print(F("<th width=50% align=""left""><input type=""text"" id=""-PD"));
      client.print(Id);
      client.print(F("-"" name=""NPPD"));
      client.print(Id);
      client.print(F("X"" value="""));
      client.print((DL[Id]/1000),DEC);
      client.print(F(""" ></th></tr></table><br>\r\n"));
      client.print(F("<input type=""submit"" value="" Submit"">\r\n"));
      client.print(F("</form>\r\n")); 
      client.println(F("<hr width=100% size=4 color=FF0000>"));
}

void StParam(){
      int LId=0;
      int io1=NetCMDS.indexOf("?",io1-1)+1;
      int io2=NetCMDS.indexOf(" ",io1);
      io1=NetCMDS.indexOf("X")-1;
      S1=NetCMDS.substring(io1, io1+1);
      LId=S1.toInt();
      io2=NetCMDS.indexOf("=", io1);
      io1=NetCMDS.indexOf(" ", io2);
      S2=NetCMDS.substring(io2+1,io1);
      DL[LId]=S2.toInt()*1000 ;
      saveConfig();
}

/*
bool SendGMA(byte IpA, String MyCmd){
  String SSd;
  WiFiClient WcGMA;
  GMA[3]=IpA;
  if (WcGMA.connect(GMA, 80)){
    WcGMA.print(MyCmd);
  }
  WcGMA.flush();
  if (WcGMA){
    while (WcGMA.connected()){
      SSd=WcGMA.readStringUntil('>');//
      return (SSd=="<OK-00");
    }
    WcGMA.flush();
    WcGMA.stop();
  }
}

*/

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

//*******************************************************    FILES    ***************************************

bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    return false;
  }

  DL[1]  =int(doc["DL1"]);
  DL[2]  =int(doc["DL2"]);
  DL[3]  =int(doc["DL3"]);
  DL[5]  =int(doc["DL5"]);
  DL[6]  =int(doc["DL6"]);
  DL[7]  =int(doc["DL7"]);
  DL[8]  =int(doc["DL8"]);

  // Real world application would store these values in some variables for
  // later use.
  return true;
}


bool saveConfig() {
  char cstr[6];
  StaticJsonDocument<200> doc;
  sprintf(cstr, "%06d", DL[1]);
  doc["DL1"] = cstr;
  sprintf(cstr, "%06d", DL[2]);
  doc["DL2"] = cstr;
  sprintf(cstr, "%06d", DL[3]);
  doc["DL3"] = cstr;
  sprintf(cstr, "%06d", DL[4]);
  doc["DL4"] = cstr;
  sprintf(cstr, "%06d", DL[5]);
  doc["DL5"] = cstr;
  sprintf(cstr, "%06d", DL[6]);
  doc["DL6"] = cstr;
  sprintf(cstr, "%06d", DL[7]);
  doc["DL7"] = cstr;
  sprintf(cstr, "%06d", DL[8]);
  doc["DL8"] = cstr;

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }
  serializeJson(doc, configFile);
  return true;
}
