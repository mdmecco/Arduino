#include <DHT.h>


//#include <ESP_EEPROM.h>
#include <ArduinoOTA.h>
//#include "A:\libmie\pulsanti.h"
#include <ESP8266WiFi.h>
//#include "A:\libraries\DHT\DHT.h"

#define DHTPIN 2                                                              
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

 
#define PRGVER "2022-11-22 V1.5"
//const String BtnColor[2] = ("green","red");


#define LogFile "GET /ghelfa/log.php?FileTS=fuori&StatoR="
#define MySIp 10
unsigned long LogFileTimer=6000;

const IPAddress staticIP(192,168,1,MySIp);
const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";
const char* ssid3 = "DGhelfa";
const char* password3 = "Lamborghini";

IPAddress GMA(192,168,1,0);
IPAddress ETH(192,168,1,6);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
IPAddress DNS(8,8,8,8);
WiFiServer server(80);
WiFiClient client;
byte WifiMas=0;  //Macchina a stati per la connessione WIFI
unsigned long WifiT0=0; // tempi di attesa per la connessione wifi
unsigned long WifiT1=0;
unsigned int WId=0;
byte WiFiClMAS=0;
unsigned long WiFiClTo=0;
bool WiFiBo=false;
String WiFiSt="";
char WiFiCh=0;

String ldl="";

byte NetMas=0;
unsigned long NetTo=0;
char NetRdC=0;
String NetCMDS="";
String NetPARS="";

byte ETHLed=0;
unsigned int ETHAdc[4];
unsigned long TSCET[4];
bool TSCBET[4];

unsigned long TSCETOpen=0;
bool TSCBETOpen=false;

unsigned long DayTimeS=0;
bool DayTimeB=false;
unsigned long DayTimeR=0;

//--------- definizione orario polli ------
#define OraApri 50400
#define OraChiudi 82800
bool APCheck=true;
bool CPCheck=true;
unsigned long MilliApriP=0;


bool rp=false;
bool OTAActive=false;

float Tem=0;
float Hum=0;

String        M1Status ="";
bool          M1StatusR=false;
unsigned long M1StatusT=0;




void setup() {
  // put your setup code here, to run once:
  //EEPROM.begin(512);
  Serial.begin(9600);
  delay(200);
  dht.begin();
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
      GetTime();
      break;
    case 10:
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
      WifiMas = 100;
      break;
    }
  //**********************************************************************************


//********** SERVER ****************************************************************
  if (WifiMas ==100){ // Rete attiva e connessa
    switch (NetMas) {
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
        NetMas=20;
        break;
      case 20:
        NetMas=0;
        //client.stop();
        if (NetCMDS=="F1"){
            Serial.print(F("<M1-ACT-01>"));
        }else if (NetCMDS== "F2"){
            Serial.print(F("<M1-ACT-02>"));
        }else if (NetCMDS== "F3"){
            Serial.print(F("<M1-ACT-03>"));
        }else if (NetCMDS== "F4"){
            Serial.print(F("<M1-ACT-04>"));
        }else if (NetCMDS== "F5"){
            Serial.print(F("<M1-ACT-05>"));
        }else if (NetCMDS== "F6"){
            Serial.print(F("<M1-ACT-06>"));
        }else if (NetCMDS== "F7"){
            Serial.print(F("<M1-ACT-07>"));
        }else if (NetCMDS== "F8"){
            Serial.print(F("<M1-ACT-08>"));
        }else if (NetCMDS== "M1STATUS"){
            client.print("<M1STATUS-");
            client.print(M1Status);
            client.print("<");
        }else if (NetCMDS== "LAMP"){
            ETH484Sw(3);
        }else if (NetCMDS== "APRIPOLLI"){
            ETH484Sw(0);  
        }else if (NetCMDS== "CHIUDIPOLLI"){
            ETH484Sw(1);
        }else if (NetCMDS== "LUCEP"){
            ETH484Sw(2);  
        }else if (NetCMDS== "STATUS"){
        
        }
        client.stop();
        break;
      case 50:  
        {
          NetTo=millis()+500;
          NetCMDS=client.readStringUntil('/');//
          NetCMDS=client.readStringUntil(' ');//
          while (client.available()){
            char c = client.read();
          }
          if (NetCMDS=="LAMPIONEXX"){
            delay(50);
            ETH484Sw(3);
            rp=true;
          }else if (NetCMDS=="LUCEPOLLIXX"){
            delay(50);
            ETH484Sw(2);
            rp=true;
          }else if (NetCMDS=="FIENILE1XX"){
            Serial.print(F("<M1-ACT-02>"));
            rp=true;
          }else if (NetCMDS=="FIENILE2XX"){
            Serial.print(F("<M1-ACT-03>"));
            rp=true;
          }else if (NetCMDS=="FIENILE3XX"){
            Serial.print(F("<M1-ACT-05>"));
            rp=true;
          }else if (NetCMDS=="FIENILE4XX"){
            Serial.print(F("<M1-ACT-06>"));
            rp=true;
          }else if (NetCMDS=="STATUS"){
            NetCMDS="<STATUS-";
            NetCMDS.concat(M1Status);
            NetCMDS.concat(">");  
            rp=true;
          }else if (NetCMDS=="OTA"){        
            rp=true;
            OTAActive= (!OTAActive);
            ArduinoOTA.begin(OTAActive);
          }else if (NetCMDS=="L1"){
            SendGMA(37,"<L1-10000>");
            rp=true;
          }else if (NetCMDS=="L2"){
            SendGMA(37,"<L2-10000>");
            rp=true;
          }else if (NetCMDS=="L3"){
            SendGMA(37,"<L3-10000>");
            rp=true;
          }else if (NetCMDS=="L4"){
            SendGMA(37,"<L4-10000>");
            rp=true;
          }else if (NetCMDS=="LUCEFOSSA"){
            SendGMA(11,"<L1-10000>");
            rp=true;
          }

          if (NetCMDS=="CFG"){
            //**************** pagina configurazione *****************************************************
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close"));  // the connection will be closed after completion of the response
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html><head><meta http-equiv=""content-type"" content=""text/html; charset=UTF-8""><title>Main Fienile</title></head>"));
            client.println(F("<body>"));
            client.print(F("<b style=""font-size:20px"">"));
            client.print(PRGVER);
            client.print("   -   "); 
            client.print(NetPARS);
            client.print(F("</b>"));
            if (OTAActive){
              client.print(F("<table style=""width:100%"" border=1> <tr><th width=100% align=""center""> OTA Active </th> </tr> </table> <hr width=100% size=4 color=0000FF> "));
            }
            client.println(F("</td></tr></tbody></table>"));
            client.println(F("</body>\r\n</html>"));
            delay(100);
            NetMas=0; // funzione del timeout di ricezione
            client.stop();
              
          }else{         
            //********************************************* pagina server inizio  *****************************************************
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close"));  // the connection will be closed after completion of the response
            if (rp){
              client.println(F("Refresh: 0;url=/"));  // reset the pages
              rp=false;
            }
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html><head><meta http-equiv=""content-type"" content=""text/html; charset=UTF-8""><title>Main Fienile</title></head>"));
            client.println(F("<body>"));
            client.print(F("<b style=""font-size:10px"">"));
            client.print(PRGVER);
            client.print(F("</b>"));
            client.println(F("<table style=""width:100%"" border=1>"));
            //client.print(F("<tr> <th width=50% align=""center"">"));
            //client.print(WiFi.SSID());
            //client.print(F(":"));
            //client.print(WiFi.RSSI());
            //client.println(F("</th>"));
            client.print(F("<th width=50% align=""center"">"));
            client.print(STime(DaySec()));
            client.println(F("</th>"));
            client.print(F("</tr></table>"));                                                                                                                                                                     
            //************************************************* Fine Header ******************************************************

            
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
  
  
            GetStatus();
            
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            client.println(BtnColor(0));
            client.println(F(";\">    <a href=\"/LUCEPOMPANAFTAXX\" > <img src=\"http://www.mdmecco.it/ghelfa/button/Luce%20nafta.gif\"   alt=\"Luce Nafta\"       style=\"width: 150px;\"></a></td>"));
            
            client.println(F("<td style=\"text-align: center; background-color:"));
            client.println(BtnColor(bitRead(ETHLed,2)));
            client.println(F(";\">        <a href=\"/LUCEPOLLIXX\">       <img src=\"http://www.mdmecco.it/ghelfa/button/Luce%20polli.gif\"   alt=\"Luce Polli\"       style=\"width: 150px;\"></a></td>"));
            
            client.println(F("<td style=\"text-align: center; background-color:"));
            client.println(BtnColor(bitRead(ETHLed,3)));
            client.println(F(";\">    <a href=\"/LAMPIONEXX\">        <img src=\"http://www.mdmecco.it/ghelfa/button/Lampione.gif\"       alt=\"Lampione\"         style=\"width: 150px;\"></a></td></tr>"));

            
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            client.println(BtnColor(StringToInt(M1Status.substring(5,6))));
            client.println(F(";\"><a href=\"/FIENILE4XX\">        <img src=\"http://www.mdmecco.it/ghelfa/button/fienile%202.gif\"    alt=\"Fuori 2\"          style=\"width: 150px;\"></a><br></td>"));
            
            client.println(F("<td style=\"text-align: center; background-color:"));
            client.println(BtnColor(StringToInt(M1Status.substring(2,3))));
            client.println(F(";\">    <a href=\"/FIENILE2XX\">        <img src=\"http://www.mdmecco.it/ghelfa/button/fienile%203.gif\"    alt=\"Fuori 1\"          style=\"width: 150px;\"></a><br></td>"));
            
            client.println(F("<td style=\"background-color:"));
            client.println(BtnColor(0));
            client.println(F("; text-align: center;\">    <a href=\"/POMPAACQUAXX\">      <img src=\"http://www.mdmecco.it/ghelfa/button/Pompa%20acqua.gif\"  alt=\"Pompa Acqua\"      style=\"width: 150px;\"></a><br></td></tr>"));
            
            client.println(F("<tr><td style=\"text-align: center; background-color:"));
            client.println(BtnColor(StringToInt(M1Status.substring(4,5))));
            client.println(F(";\"><a href=\"/FIENILE3XX\">        <img src=\"http://www.mdmecco.it/ghelfa/button/feinile%201.gif\"    alt=\"Officina\"         style=\"width: 150px;\"></a><br></td>"));
            
            client.println(F("<td style=\"text-align: center; background-color:"));
            client.println(BtnColor(StringToInt(M1Status.substring(1,2))));
            client.println(F(";\">    <a href=\"/FIENILE1XX\">        <img src=\"http://www.mdmecco.it/ghelfa/button/fienile%204.gif\"    alt=\"Ingresso Stalla\"  style=\"width: 150px;\"></a><br></td>"));
            
            client.println(F("<td style=\"text-align: center; background-color:"));
            client.println(BtnColor(0));
            client.println(F(";\">    <a href=\"/POMPANAFTAXX\">      <img src=\"http://www.mdmecco.it/ghelfa/button/Pompa%20Nafta.gif\"  alt=\"Pompa Nafta\"      style=\"width: 150px;\"></a><br></td></tr>"));
            
            client.print(F("<tr><td style=\"text-align: center;\">Temperatura:"));
            client.print(String(Tem,1));
            client.print("°C Humidita':");
            client.print(String(Hum,1));
            client.println(F("<br></td><td style=\"text-align: center;\"><br></td>"));
            /*
            client.print(F("="));
            client.print(NetCMDS);
            client.print(F("-"));
            client.print(NetPARS);
            client.print(F("="));
            */
            
            
            client.println(F("<td style=\"text-align: center;\">"));
            client.print(M1Status);
            client.println(F("</td></tr></tbody></table>"));
            client.println(F("</body>\r\n</html>"));
            
            
          //**********************************************  R1  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody><tr>"));
            client.println(F("<td style=\"text-align: center; background-color:"));
            //if (BL2==5) {
            //  client.println(BtnColor(1));
            //}else{
              client.println(BtnColor(0));
            //}
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L1\" >___L1___</a></td>"));
            client.println(F("<td style=\"text-align: center; background-color:"));
            //if (BL2==5) {
            //  client.println(BtnColor(1));
            //}else{
              client.println(BtnColor(0));
            //}
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L2\" >___L2___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          //**********************************************  R2  ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody><tr>"));
            client.println(F("<td style=\"text-align: center; background-color:"));
            //if (BL2==5) {
            //  client.println(BtnColor(1));
            //}else{
              client.println(BtnColor(0));
            //}
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L3\" >___L3___</a></td>"));
            client.println(F("<td style=\"text-align: center; background-color:"));
            //if (BL2==5) {
            //  client.println(BtnColor(1));
            //}else{
              client.println(BtnColor(0));
            //}
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L4\" >___L4___</a></td>"));
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            

          //**********************************************  R3  Luce Fossa ****************************************************************
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody><tr>"));
            client.println(F("<td style=\"text-align: center; background-color:"));
            //if (BL2==5) {
            //  client.println(BtnColor(1));
            //}else{
              client.println(BtnColor(0));
            //}
            client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/LUCEFOSSA\" >___Luce Fossa___</a></td>"));
            
            client.println(F("</tr></tbody></table>"));
          //*********************************************************************************************************************            


            
            
            
            delay(100);
            NetMas=0; // funzione del timeout di ricezione
            client.stop();
          }
          break;
        }
      case 200:
        client.stop();
        NetMas=0;
    }
//***********************************************************************
//*********************** codice dentro rete  ***************************
//***********************************************************************
  
//*********************** Legge dalla seriale ***********************
//**      <CMD-VALUES>
//*******************************************************************
    
    if (Serial.available()){
      //ldl="";
      NetCMDS=Serial.readStringUntil('<');//
      NetCMDS=Serial.readStringUntil('-');//
      NetPARS=Serial.readStringUntil('-');//
      NetPARS=Serial.readStringUntil('>');//
      //ldl=NetCMDS;
      //ldl.concat(" --  ");
      //ldl.concat(NetPARS);
      if (NetCMDS=="ET"){
        ETH484Sw(3);
      }else if (NetCMDS=="M1"){
        if (NetCMDS="REQ"){
          ldl=NetPARS;
          M1Status=NetPARS;
          M1Status.concat(String(BtnColor(0)));
          M1Status.concat(String(BtnColor(1)));
          M1Status.concat(String(BtnColor(2)));
          M1Status.concat(String(BtnColor(3)));
          //M1Status=NetCMDS;
          M1StatusR=true;
        }
      }
      NetCMDS="";
    }
//-----------  Codice dentro rete  ---------    
    for ( byte I=0; I<=3; I++){
      if ((millis() > TSCET[I]) && (TSCBET[I])){
        GetStatus();
        delay(50);
        if (bitRead(ETHLed,I)==1) {
          SwitchLamp(I+48);  
        }
        TSCBET[I]=false;
      }
    }

    if (millis() > DayTimeR){ // rinfrsco dell'orario
      GetTime();  
    }

    if ((DaySec() > OraApri) && APCheck){
      APCheck=false;
      ETH484Sw(0);
      MilliApriP=millis()+30000; 
    }

    if ((DaySec() > OraChiudi) && CPCheck && (millis()>MilliApriP)){
      CPCheck=false;
      ETH484Sw(1); 
    }

    if ((DaySec() < 30) && !CPCheck && !APCheck){
     CPCheck=true;
     APCheck=true;
    }

  if (millis() > LogFileTimer){
    String SS="";
    SS.concat(String(Tem));
    SS.concat(";");
    SS.concat(String(Hum));
    LogFileTimer=millis()+600000;
    LogCaldaia(SS);
  }



//************************************************************************
  }//*********************************************************************
//************************************ Codice fuori rete *****************


        Hum = dht.readHumidity();
        Tem = dht.readTemperature();



        if (millis() > M1StatusT){
          Serial.print(F("<M1-REQ-00>"));
          M1StatusT=millis()+500;
        }
        


}//********** fine MAIN  ***************************


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
        //Serial.print(cll);
      }
    }
    //delay(100);
    WcGMA.flush();
    WcGMA.stop();
    //Serial.println("--STOP--");
  }
}

void ETH484Sw(byte IdO){
    SwitchLamp(IdO+48);
    if (IdO==3){
      TSCET[IdO]=millis()+1800000;
    }else if (IdO==0){
      TSCET[IdO]=millis()+10000;
    }else if (IdO==1){
      TSCET[IdO]=millis()+10000;
    }else if (IdO==2){
      //TSCET[IdO]=millis()+10000;
      TSCET[IdO]=millis()+900000;
    }
    TSCBET[IdO]=true;
}

void MyPublicIP(){
  WiFiClient WcGMA;
  String Ln1="";
  //Serial.println("Connetto mdmecco...");
  if (WcGMA.connect("www.mdmecco.it", 80)){
    //Serial.println("GET");
    WcGMA.print(F("GET /ghelfa/ip.php HTTP/1.1 \r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    WcGMA.flush();
    delay(150);
    Ln1 = WcGMA.readString();
  }
  //Serial.println("");
  //Serial.println("---END---");
  WcGMA.stop();
}

/*byte GetStatusR(byte MyLIp){
  WiFiClient WcGMA;
  //IPAddress LIp[192,168,1,0];
  String Ln1="";
  if (WcGMA.connect(LIp, 80)){
    

  }
}
*/


void GetStatus(){
  byte Id=0;
  String line="";
  WiFiClient WcETH;
  if (WcETH.connect(ETH, 80)){
    WcETH.print(F("GET /status.xml"));
    WcETH.print(F(" HTTP/1.1 \r\nContent-Type: text/html\r\nConnection: close\r\nAuthorization: Basic YWRtaW46cGFzc3dvcmQ=\r\n\r\nOK\r\n"));
    WcETH.flush();
    delay(200);
    line = WcETH.readStringUntil('0');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    bitWrite(ETHLed, 0, line.toInt());
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('1');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    bitWrite(ETHLed, 1, line.toInt());
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('2');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    bitWrite(ETHLed, 2, line.toInt());
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('3');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    bitWrite(ETHLed, 3, line.toInt());
    line = WcETH.readStringUntil('7');
    line = WcETH.readStringUntil('7');
    line = WcETH.readStringUntil('v');
    line = WcETH.readStringUntil('a');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    ETHAdc[0]=line.toInt();
    line = WcETH.readStringUntil('a');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    ETHAdc[1]=line.toInt();
    line = WcETH.readStringUntil('a');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    ETHAdc[2]=line.toInt();
    line = WcETH.readStringUntil('a');
    line = WcETH.readStringUntil('>');
    line = WcETH.readStringUntil('<');
    ETHAdc[3]=line.toInt();
    WcETH.flush();
    WcETH.stop();
  }
}

void GetTime(){
  String Ln1="";
  unsigned long TT=0;
  WiFiClient WcMdMecco;
  if (WcMdMecco.connect("www.mdmecco.it", 80)){
    WcMdMecco.print(F("GET /ghelfa/time.php HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    WcMdMecco.flush();
    //delay(150);
    Ln1 = WcMdMecco.readStringUntil('<');
    Ln1 = WcMdMecco.readStringUntil('>');
    TT=atol(Ln1.c_str());
    if (TT>0){
      DayTimeB=true;
      DayTimeR=millis()+28800000;
    }else{
      DayTimeB=false;
      DayTimeR=millis()+600000;
    }
  }else{
    if (DayTimeS=0) {
      DayTimeB=false;
      DayTimeR=millis()+600000;
    }else{
      DayTimeB=true;
      DayTimeR=millis()+600000;
    }
  }
  WcMdMecco.stop();
  DayTimeS=TT - ((millis()/1000) % 86400);
}

unsigned long DaySec(){
  if (DayTimeB){
    return (((millis()/1000) + DayTimeS) % 86400);
  }else{
    return 0;
  }
}


void SwitchLamp(char Nl){  //Lampione
  WiFiClient WcETH;
  if (WcETH.connect(ETH, 80)){
    WcETH.print(F("GET /io.cgi?led="));
    WcETH.print(Nl);
    WcETH.print(F(" HTTP/1.1 \r\nContent-Type: text/html\r\nConnection: close\r\nAuthorization: Basic YWRtaW46cGFzc3dvcmQ=\r\n\r\n\r\n"));
    WcETH.flush();
    delay(100);
    while (WcETH.available()){
      char cll=WcETH.read();
      //Serial.write(cll);
    }
    WcETH.stop();
    //Serial.println("--STOP--");
  }
}

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
   }
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


void LogCaldaia(String TT){
  //  http://www.mdmecco.it/ghelfa/log.php?FileTS=FINO&StatoR=100.54
  if (client.connect("www.mdmecco.it", 80)) {
    client.print(LogFile);
    client.print(TT);
    client.print(F(" HTTP/1.1 \r\nHost: www.mdmecco.it\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"));
    client.flush();
  }
  client.stop();
}

String BtnColor (int idCol){
  if (idCol == 0){
    return "#22ff22";
  }else{
    return "red";
  }
}


long int StringToInt( String InS){
  long int dd=0;
  dd=atoi(InS.c_str());
  return dd;
}
