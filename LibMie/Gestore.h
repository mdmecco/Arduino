/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include <ArduinoJson.h>

byte Debug=0;

byte DIp=0;

#define TotalOut 8


typedef struct {
  byte IdBoard = 0;                 // Indirizzo IP della scheda
  byte fl=0;
  unsigned long TOn = 60000;        // Tempo di attivit�    
  unsigned long MillFellOff = 0;    // millis del momento di attivazione
  byte IdPinO = 0xFF;                  // Id del pin di uscita del segnale
  bool ActOption=false;           // Serve per avere pin attivi alto o basso
  String Name="";
  //char Name[10] = {20, 20, 20, 20, 20, 20,20,20,20,20};
} AOut;



typedef struct {
    byte IdBoard = 0;                 // Indirizzo IP della scheda
    byte IdOut=0;                      //Id riferimento uscita
    byte fl=0;                            //Gestione Anti-rimbalzo
    byte IdPinI = 0xFF;             //Pin di ingresso
    unsigned long TAct=0;           //gestione dell'antirimbalzo
    bool ActOption=false;           // Serve per avere pin attivi alto o basso
    String Name="";
    //char Name[10] = {20, 20, 20, 20, 20, 20,20,20,20,20};
}   AIn;


#define vIn 12
#define vOut 9

AIn   iIn[vIn];                    //array nr ingressi
AOut  iOut[vOut] ;                  //array nr uscite    


//*********************************** UDP ************************************
int UdpPort=5240;
char incomingPacket[256];
int LenUDP=0;
int IdL=0;
int Tl=0;
char InUDPL[2];
char InUDPT[5];
//****************************************************************************



// funzioni per la gestione della data e ora prese dalla rete internet tramite la wifi
unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;
unsigned long DayDate=0;


//*******************
String ShowFile="";

void WriteTime();
void ReadTime();


void HTMLStatus();


