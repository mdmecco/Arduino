/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Arduino IDE example: Examples > Arduino OTA > BasicOTA.ino
*********/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Replace with your network credentials
const char* ssid = "GGhelfa";
const char* Password = "Supergino";
const IPAddress staticIP(192,168,1,15);
const char* aHostname ="BasicOTA";

IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);




unsigned long TT=0;
bool BB=true;


//Dichiarazione di funzione che punta all'indirizzo zero
void(* Riavvia)(void) = 0;



void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.hostname(aHostname);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.hostname(aHostname);
   MDNS.begin(aHostname);
  WiFi.begin(ssid, Password);
  WiFi.hostname(aHostname);
   MDNS.begin(aHostname);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();

  if (millis()>TT) {
    TT=millis() + 100;
    BB= !BB;
    digitalWrite(LED_BUILTIN, BB);
    
  }
  
  
}
