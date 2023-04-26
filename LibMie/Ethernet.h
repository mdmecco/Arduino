/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>




EthernetClient client;
EthernetServer server(80);

byte MASEt=0;


EthernetUDP  MUdp;


bool OTAActive=false;  // per ora � messo solo per compatibilit�





//MAC Address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const IPAddress staticIP(192, 168, 1, MySIp);


bool NetConn(){
  //Innanzitutto la macchina a stati della connessione di rete
  switch (MASEt) {
    case 0:
      Ethernet.begin(mac, staticIP);
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
      break;
    case 20:
      Serial.println("Ethernet cable is connected.");
      server.begin();
      MUdp.begin(UdpPort);
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
  
    if (MASEt == 100){
        return true;
    }else{
        return false;
    }
  

}



void OTABegin(){
    //ArduinoOTA.begin(OTAActive);
}


void HeaderInfo(){

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




