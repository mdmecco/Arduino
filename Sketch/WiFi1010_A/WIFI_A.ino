
#include <SPI.h>
#include <WiFiNINA.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h

#define NMWiFi 3
String SSF="";
String DDF="";

char ssid2[] = "GGhelfa";             // your network SSID (name)
char pass2[] = "Supergino";           // your network password (use for WPA, or use as key for WEP)
char ssid0[] = "TIM-02522746";        // your network SSID (name)
char pass0[] = "6YapbMqbYNnYczXV";    // your network password (use for WPA, or use as key for WEP)
char ssid1[] = "WGhelfa";        // your network SSID (name)
char pass1[] = "Lamborghini";    // your network password (use for WPA, or use as key for WEP)
char SSIDT[] = "";

int NetId =10;
int NetSignal=-1000;


long rssi[] = {0,0,0,0};

IPAddress ip(192, 168, 1, 11);
IPAddress dns_server1(8,8,4,4);
IPAddress dns_server2(8,8,8,8);
IPAddress Getway (192,168,1,1);
IPAddress subnet (255,255,255,0);

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int MASGetConn =0;
unsigned long WTGetConn=0;

int status = WL_IDLE_STATUS;
WiFiClient client;

int WFStat=0;
byte MASConnection=0;
  



bool WifiConnection() {
  if (MASConnection==0){
    listNetworks();
    //NetId = 2;
    if (NetId == 10){
        Serial.println("****************Networks Known Network missed ****************");
        MASConnection=10; //non fare più nulla  
    } else {
      MASConnection=100;
    }
    return false;
  }else if (MASConnection==100){
    Serial.print("\t\tNetId:");
    Serial.println(NetId);
    if (NetId==0){
      WFStat=GetConnection(ssid0, pass0);
    }else if (NetId==1){
      WFStat=GetConnection(ssid1, pass1);      
    }else if (NetId==2){
      WFStat=GetConnection(ssid2, pass2);      
    }

    if (WFStat==1){ //Errore Firmware
      MASConnection=10; //non fare più nulla  
    }else if (WFStat==2){ // Timeout di connessione  
      MASConnection=10;
    }else if (WFStat==3){ // Connessione ottenuta
      printWifiStatus();
      MASConnection=2;
    }
    return false;
  }else if (MASConnection==2){
    return true;
  }else if (MASConnection==10){
    return false;
  }    
}
  

byte GetConnection (char  ssid[], char pass[]){

  Serial.print("MASGetConn==>");
  Serial.println(MASGetConn);


  switch(MASGetConn){
    case 0:
        WiFi.config(ip,dns_server1,Getway,subnet);
        status = WiFi.begin(ssid, pass);
        MASGetConn=1;
        return 0; //Avanza
        //}
        break;
    case 1:
        status=WiFi.status();
        MASGetConn=2;
        WTGetConn=millis()+10000;
        return 0; //Avanza
        break;
    case 2:
        status=WiFi.status();
        if (status == WL_CONNECTED){
          MASGetConn=4;
          return 0; //Avanza
        }else{
          MASGetConn=3;
          return 0; //Avanza
        }
        break;
    case 3:
        status=WiFi.status();
        if (status == WL_CONNECTED){
          MASGetConn=4;
          return 0; //Avanza
        }
        if (millis() > WTGetConn){
          MASGetConn=0;
          Serial.println("\t\t TimeOut");
          return 2; //cerca un'altra WIFI
        }
        break;
    case 4:
        WiFi.setDNS(dns_server1,dns_server2);
        Serial.println(String ("Connected to wifi ") + ssid );
        //digitalWrite(LED_BUILTIN, HIGH);
        return 3; //connesso
        break;
        
    default:
        return 1;
        break;
  }
}

void printWifiStatus() {
  long rssit = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssit);
  Serial.println(" dBm");
}


//******************************************************* SCAN  *******************************

void listNetworks() {
  
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    SSF=WiFi.SSID(thisNet);
    DDF=ssid0;
    if (SSF==DDF){
      if (NetSignal < WiFi.RSSI(thisNet)){
        NetSignal = WiFi.RSSI(thisNet);
        NetId=0;
      }
    }
    DDF=ssid1;
    if (SSF==DDF){
      if (NetSignal < WiFi.RSSI(thisNet)){
        NetSignal = WiFi.RSSI(thisNet);
        NetId=1;
      }
    }
    DDF=ssid2;
    if (SSF==DDF){
      if (NetSignal < WiFi.RSSI(thisNet)){
        NetSignal = WiFi.RSSI(thisNet);
        NetId=2;
      }
    }
  }
  Serial.println();
}
