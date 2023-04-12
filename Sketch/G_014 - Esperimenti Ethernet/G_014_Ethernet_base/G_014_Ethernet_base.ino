
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





//*********************************** UDP ************************************
int localUdpPort=5240;
int packetSize=0;
char incomingPacket[256];
int LenUDP=0;
int IdL=0;
int Tl=0;
char InUDPL[2];
char InUDPT[5];
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
    //while (1)
      ;
  }
  Serial.println("initialization card done.");

  SetupChannel();
  
}

void loop() {
  if (NetConn()){
    WebServer();

    
  }
  

  
//****************   UDP   **********************************************
    packetSize = MUdp.parsePacket();
    if (packetSize){
      LenUDP = MUdp.read(incomingPacket, 255);
      if (LenUDP > 5){
        InUDPL[0]=incomingPacket[0];
        if (InUDPL[0]='L') {
          InUDPL[0]=incomingPacket[1];
          InUDPL[1]=incomingPacket[2];
          IdL=atoi(InUDPL);
          /*InUDPT[0]=incomingPacket[3];
          InUDPT[1]=incomingPacket[4];
          InUDPT[2]=incomingPacket[5];
          InUDPT[3]=incomingPacket[6];
          IdL=atoi(InUDPL);
          Tl=atoi(InUDPT);
          */
          LenUDP=0;
          
        }
      }else{
        LenUDP=0;
      }
    }
//************************************************************************

  
  
  
  
  RWIoL(iLight[0]);
  RWIoL(iLight[1]);
  RWIoL(iLight[2]);
  RWIoL(iLight[3]);
  RWIoL(iLight[4]);
  RWIoL(iLight[5]);
  RWIoL(iLight[6]);
  RWIoL(iLight[7]);
  
  

}
//****************************************** FINE LOOP **************************************************
//*******************************************************************************************************
//*******************************************************************************************************
