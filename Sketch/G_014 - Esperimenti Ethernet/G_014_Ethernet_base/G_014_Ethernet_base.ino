#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "a:\libmie\mecco1.c"
//#include <ArduinoOTA.h>
#include "a:\libmie\pulsanti.c"
#include <EEPROM.h>
#include <SD.h>


#define WEBTITPAGE "G014"
#define PRGVER "2023_03_03_19"


SLight Light[8];


unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;
unsigned long DayTimeF = 0;


//MAC Address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };



// Variabili per gestione rete ed ambiente
EthernetClient client;
byte NetMas = 0;
unsigned long NetTo = 0;
char NetRdC = 0;
String NetCMDS = "";
String NetPARS = "";

String S1;
String S2;
String S3;
bool rp = false;


byte MyIp = 14;
IPAddress ip(192, 168, 1, MyIp);
byte MASEt = 0;  //Macchina a stati della connessione Ethernet, quando il valore è =100 la scheda è connessa

EthernetServer server(80);


//*********************************** UDP ************************************
EthernetUDP  MUdp;
int localUdpPort=5240;
int packetSize=0;
char incomingPacket[256];
//****************************************************************************




void setup() {
  Ethernet.init(10);  // Most Arduino shields

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("RUN Ethernet");

  if (!SD.begin(4)) {
    Serial.println("initialization card failed!");
    while (1)
      ;
  }
  Serial.println("initialization card done.");


  Light[0].IdBoard = MyIp;       // Indirizzo IP della scheda
  Light[0].fL = 0;               // Byte di funzionamento     
  Light[0].TOn = 60000;          // Tempo di attività    
  Light[0].MillFellOff = 0;      // millis del momento di attivazione
  Light[0].TAct =0 ;             // millis del momento di pressione del pulsante
  Light[0].IdPinI = 23;          // Id del pin di uscita del segnale
  Light[0].IdPinO = 22;          // Id del pin di uscita del segnale
  Light[0].Options=3;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 

  pinMode(Light[0].IdPinI,INPUT);
  pinMode(Light[0].IdPinO,OUTPUT);
  digitalWrite(Light[0].IdPinI, HIGH);

  Light[1].IdBoard = MyIp;       // Indirizzo IP della scheda
  Light[1].fL = 0;               // Byte di funzionamento     
  Light[1].TOn = 60000;          // Tempo di attività    
  Light[1].MillFellOff = 0;      // millis del momento di attivazione
  Light[1].TAct =0 ;             // millis del momento di pressione del pulsante
  Light[1].IdPinI = 25;          // Id del pin di uscita del segnale
  Light[1].IdPinO = 24;          // Id del pin di uscita del segnale
  Light[1].Options=3;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 

  pinMode(Light[1].IdPinI,INPUT);
  pinMode(Light[1].IdPinO,OUTPUT);
  digitalWrite(Light[1].IdPinI, HIGH);

  Light[2].IdBoard = MyIp;       // Indirizzo IP della scheda
  Light[2].fL = 0;               // Byte di funzionamento     
  Light[2].TOn = 60000;          // Tempo di attività    
  Light[2].MillFellOff = 0;      // millis del momento di attivazione
  Light[2].TAct =0 ;             // millis del momento di pressione del pulsante
  Light[2].IdPinI = 25;          // Id del pin di uscita del segnale
  Light[2].IdPinO = 24;          // Id del pin di uscita del segnale
  Light[2].Options=3;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 

  pinMode(Light[2].IdPinI,INPUT);
  pinMode(Light[2].IdPinO,OUTPUT);
  digitalWrite(Light[2].IdPinI, HIGH);

  Light[3].IdBoard = MyIp;       // Indirizzo IP della scheda
  Light[3].fL = 0;               // Byte di funzionamento     
  Light[3].TOn = 60000;          // Tempo di attività    
  Light[3].MillFellOff = 0;      // millis del momento di attivazione
  Light[3].TAct =0 ;             // millis del momento di pressione del pulsante
  Light[3].IdPinI = 27;          // Id del pin di uscita del segnale
  Light[3].IdPinO = 26;          // Id del pin di uscita del segnale
  Light[3].Options=3;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 

  pinMode(Light[3].IdPinI,INPUT);
  pinMode(Light[3].IdPinO,OUTPUT);
  digitalWrite(Light[3].IdPinI, HIGH);






}


void loop() {
  //Innanzitutto la macchina a stati della connessione di rete
  switch (MASEt) {
    case 0:
      Ethernet.begin(mac, ip);
      if (Ethernet.hardwareStatus() != EthernetNoHardware) {
        MASEt = 10;
      }
      break;
    case 10:
      if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is NOT connected.");
        MASEt = 15;
      } else {
        MASEt = 20;
      }
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.");
      } else if (Ethernet.hardwareStatus() == EthernetW5100) {
        Serial.println("W5100 Ethernet controller detected.");
      } else if (Ethernet.hardwareStatus() == EthernetW5200) {
        Serial.println("W5200 Ethernet controller detected.");
      } else if (Ethernet.hardwareStatus() == EthernetW5500) {
        Serial.println("W5500 Ethernet controller detected.");
      }
      break;
    case 15:
      if (Ethernet.linkStatus() != LinkOFF) {
        MASEt = 20;
      }
      MUdp.begin(localUdpPort);
      break;
    case 20:
      Serial.println("Ethernet cable is connected.");
      server.begin();
      MASEt = 100;
      //ArduinoOTA.begin(true);
      //ArduinoOTA.begin(Ethernet.localIP(), "Arduino", "password", InternalStorage);
      break;
    case 100:
      //***************************** FINE CODICE CONNESSO *************************************************
      //****************************************************************************************************
      if (Ethernet.linkStatus() == LinkOFF) {
        MASEt = 200;
        
      }
      break;
    case 200:
      Serial.println("Ethernet ERROR");
      MASEt = 0;
      break;
    default:
      MASEt = 0;
      //Ethernet.end();
  }

  if (MASEt == 100) {  // Rete attiva e connessa
                       //***************************************************************************************************
                       //***************************** Sono connesso alla rete *********************************************
                       //**********************************************************************************
                       //**********************************************************************************
                       //********** SERVER ****************************************************************
    switch (NetMas) {
      case 0:
        client = server.available();
        if ((client) & (client.connected())) {  // in ricezione
          NetMas = 5;
          NetTo = millis() + 3000;
        }
        break;
      case 5:
        if (client.available()) {
          NetRdC = client.read();
          if (NetRdC == '<') {  // comando <CMD-PAR>
            NetMas = 10;
            NetTo = millis() + 100;
          } else if (NetRdC == 'G') {
            NetMas = 50;
          } else if (NetRdC == 'P') {
            NetMas = 50;
          }
        }
        if (millis() > NetTo) {
          NetMas = 200;  // funzione del timeout di ricezione
        }
        break;
      case 10:
        NetCMDS = client.readStringUntil('-');  //
        NetPARS = client.readStringUntil('>');  //
        if (millis() > NetTo) {
          NetMas = 200;  // funzione del timeout di ricezione
        }
        NetMas = 20;
        break;
      case 20:
        if (NetCMDS == "STATUS") {
        }
        NetMas = 0;
        client.stop();
        break;
      case 50:
        {
          NetTo = millis() + 500;
          NetCMDS = client.readStringUntil('\r');
          int io1 = NetCMDS.indexOf("/SET?");
          int io2 = 0;
          if (io1 > 0) {
            rp = true;
            io1 = NetCMDS.indexOf("?", io1 - 1) + 1;
            io2 = NetCMDS.indexOf(" ", io1);
            S1 = NetCMDS.substring(io1, io2);
            io1 = 0;
            while (io1 >= 0) {
              io2 = S1.indexOf("=", io1);
              S2 = S1.substring(io1, io2);
              io1 = S1.indexOf("&", io2);
              if (io1 > 0) {
                S3 = S1.substring(io2 + 1, io1);
                io1 = io1 + 1;
              } else {
                S3 = S1.substring(io2 + 1);
              }
            }
          }
          //*************************************** LETTURA VALORE PARAMETRO ***********************************
          io1 = NetCMDS.indexOf("NPPD");
          if (io1 > 0) {
          }
          //*****************************************  Prametri  *****************************
          io1 = NetCMDS.indexOf("/PAR");
          io2 = 0;
          if (io1 > 0) {
            NetMas = 150;
            break;
          }
          //**********************************************************************************************




          //***************************************L1
          io1 = NetCMDS.indexOf("/L1 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(1, 0);
          }  // ******************************************************************

          //***************************************L2
          io1 = NetCMDS.indexOf("/L2 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(2, 0);
          }  // ******************************************************************


          //***************************************L3
          io1 = NetCMDS.indexOf("/L3 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(3, 0);
          }  // ******************************************************************

          //***************************************L4
          io1 = NetCMDS.indexOf("/L4 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(4, 0);
          }  // ******************************************************************

          //***************************************L5
          io1 = NetCMDS.indexOf("/L5 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(5, 0);
          }  // ******************************************************************

          //***************************************L6
          io1 = NetCMDS.indexOf("/L6 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(6, 0);
          }  // ******************************************************************

          //***************************************L7
          io1 = NetCMDS.indexOf("/L7 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(7, 0);
          }  // ******************************************************************

          //***************************************L8
          io1 = NetCMDS.indexOf("/L8 ");
          io2 = 0;
          if (io1 > 0) {
            rp = true;
            Slr(8, 0);
          }  // ******************************************************************


          while (client.available()) {
            char c = client.read();
          }


          //********************************************* pagina server inizio  *****************************************************
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response

          //------------------------------------------- Refresh  ***********************************************************
          if (rp) {
            client.println(F("Refresh: 0;url=/"));  // reset the pages
            rp = false;
          }
          //****************************************************************************************************************************

          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.print(F("<html><head><title>"));
          client.print(WEBTITPAGE);
          client.println(F("</title></head>"));
          client.println(F("<body>"));
          client.print(F("<b style="
                         "font-size:10px"
                         ">"));
          client.print(PRGVER);
          client.print("  --- Status:");
          client.print(Status(), BIN);
          client.print(F("</b>"));
          client.println(F("<table style="
                           "width:100%"
                           " border=1>"));
          client.print(F("<tr> <th width=50% align="
                         "center"
                         ">"));
          client.println(F("</th>"));
          client.print(F("<th width=50% align="
                         "center"
                         ">"));
          //client.print(DayDate);
          client.print(" - ");
          client.print(STime(DaySec()));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
          //************************************************* Fine Header ******************************************************

          //tabelle inizio

          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));

          client.print(F("<form action="
                         "/PAR"
                         ">"));
          client.print(F("<input type="
                         "submit"
                         " value="
                         "Parametri"
                         ">"));
          client.print(F("</form>"));

          client.print(F("<form action="
                         "/OTA"
                         ">"));
          client.print(F("<input type="
                         "submit"
                         " value="
                         "OTA"
                         ">"));
          client.print(F("</form>"));

          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));

          /*
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
*/
          client.println(F("</body>\r\n</html>"));
          delay(100);
          NetMas = 0;  // funzione del timeout di ricezione
          client.stop();
        }
        break;

      case 150:
        {
          while (client.available()) { 
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
          client.print(F("<b style="
                         "font-size:10px"
                         ">"));
          client.print(PRGVER);
          client.print("  --- Status:");
          client.print(Status(), BIN);
          client.print(F("</b>"));
          client.println(F("<table style="
                           "width:100%"
                           " border=1>"));
          client.print(F("<tr> <th width=50% align="
                         "center"
                         ">"));
          client.println(F("</th>"));
          client.print(F("<th width=50% align="
                         "center"
                         ">"));
          //          client.print(DayDate);
          client.print(" - ");
          client.print(STime(DaySec()));
          client.println(F("</th>"));
          client.print(F("</tr></table>"));
          //************************************************* Fine Header ******************************************************
          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));

          client.print(F("<form action="
                         "/"
                         ">"));
          client.print(F("<input type="
                         "submit"
                         " value="
                         "Main"
                         ">"));
          client.print(F("</form>"));

          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
          /*          ClParam(1);
          ClParam(2);
          ClParam(3);
          ClParam(4);
          ClParam(5);
          ClParam(6);
          ClParam(7);
          ClParam(8);
  */
          client.println(F("</body>\r\n</html>"));
          delay(100);
          NetMas = 200;  // funzione del timeout di ricezione
          //client.stop();
        }
        break;
      case 200:
        client.stop();
        NetMas = 0;
        break;
      default:
        NetMas = 0;
        break;
        client.stop();
    }
    if (millis() > DayTimeF) {
      DayTimeF = DayTimeF + 3600000;
      GetTime();
      Serial.println("Got Time");
      Serial.println(STime(DaySec()));
    }
  }
  //******************************************************************************************************
  //************************** Codice da eseguire sempre, dentro e fuori dalla connessione internet ******

  
  RWIoL(Light[0]);
  RWIoL(Light[1]);
  RWIoL(Light[2]);
  RWIoL(Light[3]);
  

}
//****************************************** FINE LOOP **************************************************
//*******************************************************************************************************
//*******************************************************************************************************


unsigned long DaySec() {
  if (DayTimeB) {
    return (((millis() / 1000) + DayTimeS) % 86400);
  } else {
    return 0;
  }
}


void GetTime() {
  EthernetClient WcGMA;
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

void Slr(byte Id, unsigned long Td) {
  /*if (BL[Id]!=0){
    BL[Id]=7;
  }else{
    if (Td ==0) {
      TL[Id]=DL[Id] + millis();
    }else{
      TL[Id]=(Td*1000) + millis();
    }
    BL[Id]=6;
  }
  */
}

byte Status() {
  byte Stat = 0;
  /*
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
  */
  return Stat;
}



String BtnColor(int idCol) {
  if (idCol == 0) {
    return "#22ff22";
  } else {
    return "red";
  }
}
