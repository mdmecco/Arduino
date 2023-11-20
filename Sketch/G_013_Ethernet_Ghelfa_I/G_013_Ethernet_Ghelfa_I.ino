
#define WEBTITPAGE "Ghelfa giu Casa II"
#define PRGVER "2023-10-22 V3.11 UDP"


byte MySIp= 13;
#define NrIn 8
#define NrOut 8


#include "a:\libmie\gestore.h"
#include "a:\libmie\mecco1.h"
#include "a:\libmie\Ethernet.h"
#include "a:\libmie\pulsanti_b.c"
#include "a:\libmie\PageParameter.c"

#include "a:\libmie\SD Card.c"
#include "a:\libmie\Funzioni Rete.c"


boolean fWrite = false;


void setup() {
  Serial.begin(9600);
  delay (2000);
  Ethernet.init(10);  // Most Arduino shields
  
  
  
  InitSetup();
  
  SetupChannel();
}

void loop() {
  if ( NetConn()==1 ){

    IncomingUDP();
    WebServer();
  }else{
    //Serial.println("**NetConn == 0 ");
    NetMas=1;
  }

  if (fWrite != true){
    CFGWrite();
    fWrite=true;
  }
  
  /*
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
  */
}
//****************************************** FINE LOOP **************************************************
//*******************************************************************************************************
//*******************************************************************************************************
