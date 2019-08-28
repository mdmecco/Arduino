#include <ESP8266WiFi.h>
 
const char* ssid = "SSID";
const char* password = "router_password";
 
int ledPin = 12; // GPI12 - Led Verde
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
   
  // Connessione alla rete wifi
  Serial.println();
  Serial.println();
  Serial.print("Connessione a ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connesso");
   
  // Start the server
  server.begin();
  Serial.println("Server attivato");
 
  // Mostra l'indirizzo IP assegnato
  Serial.print("Utilizza questa URL per connetterti: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
    
}
 
void loop() {
  // Controlla se il client è connesso
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Aspetta finchè il client non manda qualche dato
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
   
  // Leggi la parte iniziale della richiesta (fino al primo ritorno a capo)
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
   
  // identifica il tipo di richiesta
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
   
 
  // Ritorna la risposta del webserver
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println(""); //  serve assolutamente
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
   
  client.print("Il led adesso è: ");
   
  if(value == HIGH) {
    client.print("acceso");  
  } else {
    client.print("spento");
  }
  client.println("<br><br>");
  client.println("Clicca <a href=\"/LED=ON\">qui</a> per accendere il led verde<br>");
  client.println("Clicca <a href=\"/LED=OFF\">qui</a> per spegnere il led verde<br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconesso");
  Serial.println("");
 
}
