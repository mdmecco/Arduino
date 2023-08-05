
#define WEBTITPAGE "Abat Jour Daria"
#define PRGVER "2023-08-06 V3 UDP"
#define MySIp 16

#include "a:\libmie\gestore.h"
#include "a:\libmie\mecco1.h"
#include "a:\libmie\wifi.h"
//#include "a:\libmie\pulsanti.h"
#include "a:\libmie\pulsanti_b.c"
#include "a:\libmie\PageParameter.c"
#include "a:\libmie\SD Card.c"
#include "a:\libmie\Funzioni Rete.c"




byte d =0;

int AdcValue =0;

//Pulsanti
//16 Rosso
// 4 Verde
// 0 Blue
//15 Giallo


void setup() { 
  LittleFS.begin();
  Serial.begin(9600);

  delay(2000);
  
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);
  
  SetupChannel();

   //if (!loadConfig()){
  /*
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

      
   //}  
}



void loop() {
  
  if (NetConn()){
    WebServer();
    IncomingUDP();

    
    if (millis() > DayTimeR) {
      GetTime();
      GetDate();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    }

    
    

  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************

  
  RWIO(0);
  RWIO(1);
  RWIO(2);
  RWIO(3);

  wOut(0);

//**************************************************************************************
}
//********** fine MAIN  ***************************************************************
//**************************************************************************************
