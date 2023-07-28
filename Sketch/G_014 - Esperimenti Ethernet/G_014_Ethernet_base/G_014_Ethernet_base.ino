
#define WEBTITPAGE "Ghelfa giu Casa II"
#define PRGVER "2023-05-17 V1 UDP"
#define MySIp 14


#include "a:\libmie\gestore.h"
#include "a:\libmie\mecco1.h"
#include "a:\libmie\Ethernet.h"
#include "a:\libmie\pulsanti_b.c"
#include "a:\libmie\PageParameter.c"

#include "a:\libmie\SD Card.c"
#include "a:\libmie\Funzioni Rete.c"


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Ethernet.init(10);  // Most Arduino shields
  Serial.println("RUN Ethernet");
   InitSetup();
  SetupChannel();
}

void loop() {
  if ( NetConn()==1 ){

    IncomingUDP();
    WebServer();
  }else{
    Serial.println("**NetConn == 0 ");
    NetMas=1;
  }
  
  
  RWIO(4);
  RWIO(5);
  RWIO(6);
  RWIO(7);
  RWIO(8);
  RWIO(9);
  RWIO(10);
  RWIO(11);

  wOut(1);
  wOut(2);
  wOut(3);
  wOut(4);
  wOut(5);
  wOut(6);
  wOut(7);
  wOut(8);

}
//****************************************** FINE LOOP **************************************************
//*******************************************************************************************************
//*******************************************************************************************************
