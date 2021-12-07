#include <ESP8266WiFi.h>

const char* ssid1 = "GGhelfa";
const char* password1 = "Supergino";
const char* ssid2 = "WGhelfa";
const char* password2 = "Lamborghini";



IPAddress staticIP(192,168,1,10);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);


void setup()
{
  
  Serial.begin(115200);
  
}


// prepare a web page to be send to a client (web browser)
String prepareHtmlPage()
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "Analog input:  " + String(analogRead(A0)) +
            "</html>" +
            "\r\n";
  return htmlPage;
}


void loop()
{
  int WId=0;
  while (WiFi.status() == WL_CONNECTED){
      WiFiClient client = server.available();
      // wait for a client (web browser) to connect
      if (client)
      {
        Serial.println("\n[Client connected]");
        while (client.connected())
        {
          // read line by line what the client (web browser) is requesting
          if (client.available())
          {
            String line = client.readStringUntil('\r');
            Serial.print(line);
            // wait for end of client's request, that is marked with an empty line
            if (line.length() == 1 && line[0] == '\n')
            {
              delay(30);
              client.println(prepareHtmlPage());
              Serial.println("[Sent data]");
              break;
            }
          }
        }
        delay(100); // give the web browser time to receive the data
    
        // close the connection:
        client.stop();
        Serial.println("[Client disonnected]");
      }
  }
  WiFi.disconnect();
  WId=WIFIScan();
  Serial.print("WId Selected:");
  Serial.println(WId);
  if (WId > 0){
    bool CWF=true;
    WiFi.config(staticIP, gateway, subnet);
    WiFi.hostname("ESP_01");
    Serial.println("Config Done");
    if (WId==1){
      WiFi.begin(ssid1, password1);
    }
    if (WId==2){
      WiFi.begin(ssid2, password2);
    }
    Serial.println("Begin....");
    unsigned long tt0=millis();
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print("."); 
      if (millis()>(tt0+40000)){
        CWF=false;
        break;
      }
    }
    if (CWF == true) {
      Serial.println("Connected....");
      server.begin();
      Serial.println("Server started...");
    }
   } 
}


// WiFi.scanNetworks will return the number of networks found
// It select witch one have a better signal for connection
int WIFIScan() {
		int RssV=-1000;
		int WId=0;
		Serial.println("scan start");

		int n = WiFi.scanNetworks();
		Serial.println("scan done");
		if (n == 0){
			Serial.println("no networks found");
		}else{
			Serial.print(n);
			Serial.println(" networks found");
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
				Serial.print(i + 1);
				Serial.print(": ");
				Serial.print(WiFi.SSID(i));
				Serial.print(" (");
				Serial.print(WiFi.RSSI(i));
				Serial.print(")");
				Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
			  delay(10);
			}
			Serial.print(" WiFi selected:");
			Serial.println(WId);
		}
		return WId;
    }
