#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>


byte WifiMas=0;  //Macchina a stati per la connessione WIFI
unsigned long WifiT0=0; // tempi di attesa per la connessione wifi
unsigned long WifiT1=0;

const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";
const char* ssid3 = "DGhelfa";
const char* password3 = "Lamborghini";

IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

unsigned int WId=0;
WiFiClient client;

WiFiServer server(80);


bool WifiGhelfa (IPAddress MyIp){
  //***************** CONNESSIONE WIFI ******************************************************
    if (WifiMas==0) {
      WifiMas=1;
      return false;
    }else if (WifiMas ==1){
      if (WIFIScan()>0) {
        WiFi.config(MyIp, gateway, subnet);
        WiFi.hostname("PFM_19");
        if (WId==1){
          WiFi.begin(ssid1, password1);
          WifiMas=4; 
        }else if (WId==2){
          WiFi.begin(ssid2, password2);
          WifiMas=4;
        }else if (WId==3){
          WiFi.begin(ssid3, password3);
          WifiMas=4;    
        }else{
          WifiMas=2;
        }     
      }
      return false;
    }else if (WifiMas ==2){
      WiFi.disconnect();
      WifiMas=10;
      return false;
    }else if (WifiMas ==4){
      WifiT0=millis()+6000;
      WifiMas=5;
      return false;
    }else if (WifiMas ==5){  
      if (WiFi.status() == WL_CONNECTED){
        WifiMas=6;
      }
      if (millis()> WifiT0 ) {
        WifiMas=10;
      }
      return false;
    }else if (WifiMas ==6){  
      server.begin();
      client = server.available();
      WifiMas=100;
      ArduinoOTA.begin();
      return false;
    }else if (WifiMas ==10){
      WifiT0=millis()+5000;
      WifiMas=11;
      return false;
    }else if (WifiMas ==11){
      if (millis()> WifiT0) {
        ArduinoOTA.begin(false);
        server.close();
        WiFi.disconnect();
        WifiMas=0;
      }
      return false;
    }else if (WifiMas ==100){
      ArduinoOTA.handle();
      client = server.available();
      if (WiFi.status() != WL_CONNECTED){
        WifiMas=10;
      }
      return true;
    }
}

int WIFIScan() {                                                                           //Scan delle Wifi
  int RssV=-1000;
  WId=0;
  WiFi.scanDelete();
  int n = WiFi.scanNetworks();
  if (n == 0){
  }else{
   for (int i = 0; i < n; ++i){
    if (WiFi.SSID(i)=="GGhelfa"){
     if (WiFi.RSSI(i) > RssV){
      RssV=WiFi.RSSI(i);
      WId=1;
     }
    }
    if (WiFi.SSID(i)=="WGhelfa"){
     if (WiFi.RSSI(i)> RssV){
      RssV=WiFi.RSSI(i);
      WId=2;
     }
    }
    if (WiFi.SSID(i)=="DGhelfa"){
     if (WiFi.RSSI(i)> RssV){
      RssV=WiFi.RSSI(i);
      WId=3;
     }
    }
    // delay(10);
   }
  }
  return WId;
}
