#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "a:\libmie\pulsanti.h"
#include "a:\libmie\mecco1.h"


#define WEBTITPAGE "Abat Jour Daria"
#define PRGVER "2023-01-17 V1.0 UDP"
#define MySIp 16

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

int L[9] ={2,16,4,0,15,13,12,14,5};

unsigned long TL[17] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//Durate di default in millis della accensioni delle singole uscite
unsigned long TOnAct[5] ={10000,10000,10000,10000,10000};
byte Bf[5] ={0,0,0,0,0};

unsigned long TAct = 20;

byte d =0;

int AdcValue =0;


void setup() { 
  LittleFS.begin();
  Serial.begin(9600);
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);
  pinMode(L[0], OUTPUT);
  pinMode(L[1], INPUT);
  pinMode(L[2], INPUT);
  pinMode(L[3], INPUT);
  pinMode(L[4], INPUT);
  pinMode(L[5], OUTPUT);
  
  digitalWrite(L[0], Spento);
  digitalWrite(L[5], Spento);
  //digitalWrite(L[1], HIGH);
  /*
  digitalWrite(L[2], Spento);
  digitalWrite(L[3], Spento);
  digitalWrite(L[4], Spento);
  digitalWrite(L[5], Spento);
  digitalWrite(L[6], Spento);
  digitalWrite(L[7], Spento);
  digitalWrite(L[8], Spento);
  digitalWrite(L[9], Spento);
  */

   if (!loadConfig()){
      TOnAct[1] = 20000;
      TOnAct[2] = 20000;
      TOnAct[3] = 20000;
      TOnAct[4] = 20000;
      TOnAct[5] = 20000;
      /*TOnAct[6] = 20;
      TOnAct[7] = 20;
      TOnAct[8] = 20;
      DL[9] = 20;
      DL[11] = 20;
      DL[12] = 20;
      DL[13] = 20;
      DL[14] = 20;
      DL[15] = 20;
      DL[16] = 20;*/
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
          
        }else if (NetCMDS=="L1"){
          //Slr(1,GetValue(NetPARS));
        }else if (NetCMDS=="L2"){
          //Slr(2,GetValue(NetPARS));
        }else if (NetCMDS=="L3"){
          //Slr(3,GetValue(NetPARS));
        }else if (NetCMDS=="L4"){
          //Slr(4,GetValue(NetPARS));
        }else if (NetCMDS=="L5"){
          //Slr(5,GetValue(NetPARS));
        }else if (NetCMDS=="L6"){
          //Slr(6,GetValue(NetPARS));
        }else if (NetCMDS=="L7"){
          //Slr(7,GetValue(NetPARS));
        }else if (NetCMDS=="L8"){
          //
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
          } // ******************************************************************
          
          //***************************************L2
          io1=NetCMDS.indexOf("/L2 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************


          //***************************************L3
          io1=NetCMDS.indexOf("/L3 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L4
          io1=NetCMDS.indexOf("/L4 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L5
          io1=NetCMDS.indexOf("/L5 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L6
          io1=NetCMDS.indexOf("/L6 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L7
          io1=NetCMDS.indexOf("/L7 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L8
          io1=NetCMDS.indexOf("/L8 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L9
          io1=NetCMDS.indexOf("/L9 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L10
          io1=NetCMDS.indexOf("/L10 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L11
          io1=NetCMDS.indexOf("/L11 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L12
          io1=NetCMDS.indexOf("/L12 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L13
          io1=NetCMDS.indexOf("/L13 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L14
          io1=NetCMDS.indexOf("/L14 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L15
          io1=NetCMDS.indexOf("/L15 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L16
          io1=NetCMDS.indexOf("/L16 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            
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

            client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
            
            client.print(F("<form action=""/PAR"">"));
            client.print(F("<input type=""submit"" value=""Parametri"">"));
            client.print(F("</form>")); 
      
            client.print(F("<form action=""/OTA"">"));
            client.print(F("<input type=""submit"" value=""OTA"">"));
            client.print(F("</form>")); 

            client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            client.println(F("<tr>"));
            TButton(1);
            TButton(2);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(3);
            TButton(4);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(5);
            TButton(6);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(7);
            TButton(8);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(9);
            TButton(10);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(11);
            TButton(12);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(13);
            TButton(14);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(15);
            TButton(16);
            client.println(F("</tr>"));
            client.println(F("</tbody></table>"));
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
          client.print(F("<table style=""width:100%"" border=1>"));
          client.println(F("<tr>"));
          ClParam(1);
          ClParam(2);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(3);
          ClParam(4);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(5);
          ClParam(6);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(7);
          ClParam(8);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(9); 
          ClParam(10);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(11);
          ClParam(12);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(13);
          ClParam(14);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(15);
          ClParam(16);
          client.println(F("</tr>"));
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
        }
      }
    }

//************************************************************************

    if (millis() > DayTimeR) {
      GetTime();
      GetDate();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    }

    /*

    bitWrite(Bf[1],0 , (digitalRead(L[1])==0));
    Serial.println(digitalRead(L[1]));
    /*
    if (digitalRead(L[1])==0){
      Serial.println(L[1]);
    }else{
      Serial.println("-");
    }
    */
    //Serial.println(5,BIN);
    
    bitWrite(Bf[1],0 , (digitalRead(L[1])==0));
    ProcBtn(Bf[1], TAct, TOnAct[1], TL[1]);
     if ((bitRead(Bf[1],7))==1) {
      MUdp.beginPacket("192.168.1.13",localUdpPort );
      MUdp.write("L1-00000");
      MUdp.endPacket();
    }

    bitWrite(Bf[2],0 , (digitalRead(L[2])==0));
    ProcBtn(Bf[2], TAct, TOnAct[2], TL[2]);
     if ((bitRead(Bf[2],7))==1) {
      MUdp.beginPacket("192.168.1.10",localUdpPort );
      MUdp.write("L1-00000");
      MUdp.endPacket();
    }

    bitWrite(Bf[4],0 , (digitalRead(L[4])==0));
    ProcBtn(Bf[4], TAct, TOnAct[4], TL[4]);
    digitalWrite(L[5],(bitRead(Bf[4],1)));




    //}else{
    //  Bl[17]=0;
    //}
    /*
    if ((digitalRead(L[2]))==Acceso) {
      if (Bl[16]==0){ //C
        MUdp.beginPacket("192.168.1.13",localUdpPort );
        MUdp.write("L2-00000");
        MUdp.endPacket();
        Bl[16]=1;
      }
    }else{
      Bl[16]=0;
    }
      if ((digitalRead(L[3]))==Acceso) {
        if (Bl[15]==0){ //B
          MUdp.beginPacket("192.168.1.13",localUdpPort );
          MUdp.write("L3-00000");
          MUdp.endPacket();
          Bl[15]=1;
        }
      }else{
        Bl[15]=0;
      }
      if ((digitalRead(L[4]))==Acceso) {
        if (Bl[14]==0){ //A
          MUdp.beginPacket("192.168.1.13",localUdpPort );
          MUdp.write("L4-00000");
          MUdp.endPacket();
          Bl[14]=1;
        }
      }else{
        Bl[14]=0;
      }
  
      */
  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************

/*
  Olr(1);
  Olr(2);
  Olr(3);
  Olr(4);
  Olr(5);
  Olr(6);
  Olr(7);
  Olr(8);
 */

  //AdcValue=analogRead(A0);
  //Serial.print("ADC Value: ");
  //Serial.println(AdcValue);

  


//**************************************************************************************
}
//********** fine MAIN  ***************************************************************
//**************************************************************************************
//------------------------------------------------- Funzioni --------------------------


void ClParam(byte Id){
      client.print(F("<td><form action=""/PAR"">\r\n"));
      client.print(F("<label>Durata Accensione in secondi canale:"));
      client.print(Id);
      client.print(F("</label><input type=""text"" id=""-PD"));
      client.print(Id);
      client.print(F("-"" name=""NPPD"));
      client.print(Id);
      client.print(F("X"" value="""));
      client.print((TOnAct[Id]/1000),DEC);
      client.print(F(""" >"));
      client.print(F("<input type=""submit"" value="" Submit"">\r\n"));
      client.print(F("</form>\r\n</td>\r\n"));
}

void StParam(){
      int LId=0;
      int io1=NetCMDS.indexOf("/PAR?NPPD")+9;
      int io2=NetCMDS.indexOf("X");
      S1=NetCMDS.substring(io1, io2);
      LId=S1.toInt();
      io2=NetCMDS.indexOf("=", io1);
      io1=NetCMDS.indexOf(" ", io2);
      S2=NetCMDS.substring(io2+1,io1);
      TOnAct[LId]=S2.toInt()*1000 ;
      saveConfig();
}


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

  TOnAct[1]  =int(doc["DL1"]);
  TOnAct[2]  =int(doc["DL2"]);
  TOnAct[3]  =int(doc["DL3"]);
  TOnAct[4]  =int(doc["DL4"]);
  /*
  TOnAct[5]  =int(doc["DL5"]);
  DL[6]  =int(doc["DL6"]);
  DL[7]  =int(doc["DL7"]);
  DL[8]  =int(doc["DL8"]);
  DL[9]  =int(doc["DL9"]);
  DL[10]  =int(doc["DL10"]);
  DL[11]  =int(doc["DL11"]);
  DL[12]  =int(doc["DL12"]);
  DL[13]  =int(doc["DL13"]);
  DL[14]  =int(doc["DL14"]);
  DL[15]  =int(doc["DL15"]);
  DL[16]  =int(doc["DL16"]); */


  // Real world application would store these values in some variables for
  // later use.
  return true;
}


bool saveConfig() {
  char cstr[6];
  StaticJsonDocument<200> doc;
  sprintf(cstr, "%06d", TOnAct[1]);
  doc["DL1"] = cstr;
  sprintf(cstr, "%06d", TOnAct[2]);
  doc["DL2"] = cstr;
  sprintf(cstr, "%06d", TOnAct[3]);
  doc["DL3"] = cstr;
  sprintf(cstr, "%06d", TOnAct[4]);
  doc["DL4"] = cstr;
  /*
  sprintf(cstr, "%06d", DL[5]);
  doc["DL5"] = cstr;
  sprintf(cstr, "%06d", DL[6]);
  doc["DL6"] = cstr;
  sprintf(cstr, "%06d", DL[7]);
  doc["DL7"] = cstr;
  sprintf(cstr, "%06d", DL[8]);
  doc["DL8"] = cstr;
  sprintf(cstr, "%06d", DL[9]);
  doc["DL9"] = cstr;
  sprintf(cstr, "%06d", DL[10]);
  doc["DL10"] = cstr;
  sprintf(cstr, "%06d", DL[11]);
  doc["DL11"] = cstr;
  sprintf(cstr, "%06d", DL[12]);
  doc["DL12"] = cstr;
  sprintf(cstr, "%06d", DL[13]);
  doc["DL13"] = cstr;
  sprintf(cstr, "%06d", DL[14]);
  doc["DL14"] = cstr;
  sprintf(cstr, "%06d", DL[15]);
  doc["DL15"] = cstr;
  sprintf(cstr, "%06d", DL[16]);
  doc["DL16"] = cstr; */
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }
  serializeJson(doc, configFile);
  return true;
}

void TButton (byte Id ){
  String Sid= String(Id, DEC);
  //**********************************************  L5  ****************************************************************
    client.println(F("<td style=\"text-align: center; background-color:"));
    client.println(BtnColor(0));
    client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L"));
    client.println(Sid);
    client.println(F("\" >___L"));
    client.println(Sid);
    client.println(F("___</a></td>"));
  //*********************************************************************************************************************            
}
