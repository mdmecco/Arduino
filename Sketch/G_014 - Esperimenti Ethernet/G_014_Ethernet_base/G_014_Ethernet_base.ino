
#define WEBTITPAGE "Ghelfa giu Casa II"
#define PRGVER "2023-04-06 V1.0 UDP"
#define MySIp 14


#include "a:\libmie\mecco1.h"
#include "a:\libmie\Ethernet.h"
#include "a:\libmie\gestore.h"
//#include "a:\libmie\pulsanti.h"
#include "a:\libmie\pulsanti_b.c"
#include "a:\libmie\PageParameter.c"
#include "a:\libmie\Funzioni Rete.c"
#include "a:\libmie\SD Card.c"






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
    //while (1)
      ;
  }
  Serial.println("initialization card done.");

  SetupChannel();
  
}

void loop() {
  if (NetConn()){
    WebServer();
    IncomingUDP();
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
