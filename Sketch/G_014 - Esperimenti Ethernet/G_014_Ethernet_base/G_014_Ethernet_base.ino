#include <SPI.h>
#include <Ethernet.h>
#include "a:\libmie\mecco1.h"
#include <ArduinoOTA.h>



unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;
unsigned long DayTimeF = 0;


//MAC Address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};


byte MyIp=14;
IPAddress ip(192, 168, 1, MyIp);
byte MASEt=0; //Macchina a stati della connessione Ethernet, quando il valore è =100 la scheda è connessa

EthernetServer server(80);


void setup() {
  Ethernet.init(10);  // Most Arduino shields

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("RUN.........");
}


void loop() {
  //Innanzitutto la macchina a stati della connessione di rete
  switch (MASEt){
    case 0:
      Ethernet.begin(mac, ip);
      if (Ethernet.hardwareStatus() != EthernetNoHardware) {
        MASEt=10;
      }
      break;
    case 10:
      if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is NOT connected.");
        MASEt=15;
      }else{
        MASEt=20;
      }
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
          Serial.println("Ethernet shield was not found.");
        }
        else if (Ethernet.hardwareStatus() == EthernetW5100) {
          Serial.println("W5100 Ethernet controller detected.");
        }
        else if (Ethernet.hardwareStatus() == EthernetW5200) {
          Serial.println("W5200 Ethernet controller detected.");
        }
        else if (Ethernet.hardwareStatus() == EthernetW5500) {
          Serial.println("W5500 Ethernet controller detected.");
        }
      break;
    case 15:
      if (Ethernet.linkStatus() != LinkOFF) {
        MASEt=20;
      }
      break;
    case 20:
      Serial.println("Ethernet cable is connected.");
      server.begin();
      MASEt=100;
      //ArduinoOTA.begin(true);
      ArduinoOTA.begin(Ethernet.localIP(), "Arduino", "password", InternalStorage);
      break;
    case 100:
      ArduinoOTA.poll();
      if (Ethernet.linkStatus() == LinkOFF) {
        MASEt=200;       
      }
//***************************************************************************************************
//***************************** Sono connesso alla rete *********************************************
    //Controllo se c'è qualche cosa sulla rete
    EthernetClient client = server.available();
    if (client) {
      Serial.println("new client");





  
      client.stop();
    }

    
    if (millis() > DayTimeF) {
        DayTimeF=DayTimeF+3600000;
        GetTime();
        Serial.println("Got Time");  
        Serial.println(STime(DaySec()));
       
    }


//***************************** FINE CODICE CONNESSO *************************************************
//****************************************************************************************************      
      break;
    case 200:
      Serial.println("Ethernet ERROR");
      MASEt=0;
      break;
    default:
      MASEt=0;
      //Ethernet.end();
  }
//******************************************************************************************************
//************************** Codice da eseguire sempre, dentro e fuori dalla connessione internet ******


  
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
