
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

  
  iLight[1].LoPinI=4;   //Verde
  iLight[2].LoPinI=16; // Rosso
  iLight[3].LoPinI=0; // Blue
  
  SetupChannel();
  

  
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
  
  if (NetConn()){
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
