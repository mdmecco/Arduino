
#define WEBTITPAGE "Abat Jour Daria"
#define PRGVER "2023-01-17 V1.1 UDP"
#define MySIp 16


#include "a:\libmie\wifi.h"
#include "a:\libmie\pulsanti.h"
#include "a:\libmie\gestore.h"
#include "a:\libmie\mecco1.h"
#include "a:\libmie\pulsanti_b.c"
#include "a:\libmie\PageParameter.c"
#include "a:\libmie\Funzioni Rete.c"

SLight iLight[5] ;


//*********************************** UDP ************************************

int localUdpPort=5240;
int packetSize=0;
char incomingPacket[256];
int LenUDP=0;
int IdL=0;
int Tl=0;
char InUDPL[2]={0,0};
char InUDPT[5]={0,0,0,0,0};
//****************************************************************************


/*
#define ArrayLenght 255
float TmpArray[ArrayLenght]={0};
unsigned long ArrayTime=0;
unsigned int ArrayI=0;
#define ArrayRefresh 600000
*/


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
  OTAActive=false;
  WiFi.hostname(WEBTITPAGE);
  
  //Lammpada locale
  iLight[0].IdBoard = MySIp;       // Indirizzo IP della scheda
  iLight[0].fL = 0;               // Byte di funzionamento     
  iLight[0].TOn = 60000;          // Tempo di attività    
  iLight[0].MillFellOff = 0;      // millis del momento di attivazione
  iLight[0].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[0].IdPinI = 15;          // (Giallo) Id del pin di uscita del segnale
  iLight[0].IdPinO = 13;          // Id del pin di uscita del segnale
  iLight[0].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[0]);
  

  //Lammpada locale
  iLight[1].IdBoard = MySIp;       // Indirizzo IP della scheda
  iLight[1].fL = 0;               // Byte di funzionamento     
  iLight[1].TOn = 60000;          // Tempo di attività    
  iLight[1].MillFellOff = 0;      // millis del momento di attivazione
  iLight[1].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[1].IdPinI = 0;          // (Blu) Id del pin di ingresso del segnale (Pulsante)
  iLight[1].IdPinO = 0xFF;          // Id del pin di uscita del segnale (Uscita luce )
  iLight[1].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[1]);


  //Lammpada locale
  iLight[2].IdBoard = MySIp;       // Indirizzo IP della scheda
  iLight[2].fL = 0;               // Byte di funzionamento     
  iLight[2].TOn = 60000;          // Tempo di attività    
  iLight[2].MillFellOff = 0;      // millis del momento di attivazione
  iLight[2].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[2].IdPinI = 4;          // (Verde) Id del pin di ingresso del segnale (Pulsante)
  iLight[2].IdPinO = 0xFF;          // Id del pin di uscita del segnale (Uscita luce )
  iLight[2].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[2]);

  //Lammpada locale
  iLight[3].IdBoard = MySIp;       // Indirizzo IP della scheda
  iLight[3].fL = 0;               // Byte di funzionamento     
  iLight[3].TOn = 60000;          // Tempo di attività    
  iLight[3].MillFellOff = 0;      // millis del momento di attivazione
  iLight[3].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[3].IdPinI = 16;          // (Rosso) Id del pin di ingresso del segnale (Pulsante)
  iLight[3].IdPinO = 0xFF;          // Id del pin di uscita del segnale (Uscita luce )
  iLight[3].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[3]);

 
   if (!loadConfig()){
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

      
   }  
}



void loop() {
  
  if (WifiConn()){
    WebServer();
    
//*********************** codice dentro rete  ***************************
//***********************************************************************
   
//****************   UDP   **********************************************
    packetSize = MUdp.parsePacket();
    if (packetSize){
      LenUDP = MUdp.read(incomingPacket, 255);
      if (LenUDP > 5){
        InUDPL[0]=incomingPacket[0];
        if (InUDPL[0]='L') {
          InUDPL[0]=incomingPacket[1];
          InUDPL[1]=incomingPacket[2];
          InUDPT[0]=incomingPacket[3];
          InUDPT[1]=incomingPacket[4];
          InUDPT[2]=incomingPacket[5];
          InUDPT[3]=incomingPacket[6];
          Tl=atoi(InUDPT);
          IdL=atoi(InUDPL);
          LenUDP=0;
        }
      }else{
        LenUDP=0;
      }
    }
//************************************************************************

    if (millis() > DayTimeR) {
      GetTime();
      GetDate();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    }


  
//**************************************************************************************  
  } //**********************************************************************************
// ************************   Codice fuori rete ****************************************


  RWIoL(iLight[0]);


//**************************************************************************************
}
//********** fine MAIN  ***************************************************************
//**************************************************************************************
//------------------------------------------------- Funzioni --------------------------







unsigned long GetValue (String Vs){
  unsigned long dd=0;
  dd= Vs.toInt();
  dd=dd*1000;
  return dd;
}



String BtnColor (int idCol){
  if (idCol == 0){
    return "#22ff22";
  }else{
    return "red";
  }
}

//*******************************************************    FILES    ***************************************

bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    return false;
  }

/*
  TOnAct[1]  =int(doc["DL1"]);
  TOnAct[2]  =int(doc["DL2"]);
  TOnAct[3]  =int(doc["DL3"]);
  TOnAct[4]  =int(doc["DL4"]);
  /*
  TOnAct[5]  =int(doc["DL5"]);
  DL[6]  =int(doc["DL6"]);
  DL[7]  =int(doc["DL7"]);
  DL[8]  =int(doc["DL8"]);
  DL[9]  =int(doc["DL9"]);
  DL[10]  =int(doc["DL10"]);
  DL[11]  =int(doc["DL11"]);
  DL[12]  =int(doc["DL12"]);
  DL[13]  =int(doc["DL13"]);
  DL[14]  =int(doc["DL14"]);
  DL[15]  =int(doc["DL15"]);
  DL[16]  =int(doc["DL16"]); */


  // Real world application would store these values in some variables for
  // later use.
  return true;
}


bool saveConfig() {
  char cstr[6];
  StaticJsonDocument<200> doc;
  //sprintf(cstr, "%06d", TOnAct[1]);
  //doc["DL1"] = cstr;
  //sprintf(cstr, "%06d", TOnAct[2]);
  //doc["DL2"] = cstr;
  //sprintf(cstr, "%06d", TOnAct[3]);
  //doc["DL3"] = cstr;
  //sprintf(cstr, "%06d", TOnAct[4]);
  //doc["DL4"] = cstr;
  /*
  sprintf(cstr, "%06d", DL[5]);
  doc["DL5"] = cstr;
  sprintf(cstr, "%06d", DL[6]);
  doc["DL6"] = cstr;
  sprintf(cstr, "%06d", DL[7]);
  doc["DL7"] = cstr;
  sprintf(cstr, "%06d", DL[8]);
  doc["DL8"] = cstr;
  sprintf(cstr, "%06d", DL[9]);
  doc["DL9"] = cstr;
  sprintf(cstr, "%06d", DL[10]);
  doc["DL10"] = cstr;
  sprintf(cstr, "%06d", DL[11]);
  doc["DL11"] = cstr;
  sprintf(cstr, "%06d", DL[12]);
  doc["DL12"] = cstr;
  sprintf(cstr, "%06d", DL[13]);
  doc["DL13"] = cstr;
  sprintf(cstr, "%06d", DL[14]);
  doc["DL14"] = cstr;
  sprintf(cstr, "%06d", DL[15]);
  doc["DL15"] = cstr;
  sprintf(cstr, "%06d", DL[16]);
  doc["DL16"] = cstr; */
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }
  serializeJson(doc, configFile);
  return true;
}

void TButton (byte Id ){
  String Sid= String(Id, DEC);
  //**********************************************  L5  ****************************************************************
    client.println(F("<td style=\"text-align: center; background-color:"));
    client.println(BtnColor(0));
    client.println(F(";\"> <font face=\"Times New Roman\" size=\"+5\" >  <a href=\"/L"));
    client.println(Sid);
    client.println(F("\" >___L"));
    client.println(Sid);
    client.println(F("___</a></td>"));
  //*********************************************************************************************************************            
}
