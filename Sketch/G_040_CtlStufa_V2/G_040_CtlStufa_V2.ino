#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_MAX31865.h>

Adafruit_MAX31865 T1 = Adafruit_MAX31865(41);
Adafruit_MAX31865 T2 = Adafruit_MAX31865(42);
Adafruit_MAX31865 T3 = Adafruit_MAX31865(43);
Adafruit_MAX31865 T4 = Adafruit_MAX31865(44);


#define RREF      430.0
#define RNOMINAL  100.0


long tT1 = 0;
long tT2 = 0;
long tT3 = 0;
long tT4 = 0;

byte IdT=0;
unsigned long TimerDeltaReadT=0;
unsigned long DeltaRead=10000;

byte MAS =0;




// Modifica con i dettagli della tua rete
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 40);
EthernetServer server(80);
byte EthMAS=0;
unsigned long EthTimeDisconnect=0;

byte BackC=0;

void setup() {
  Serial.begin(9600);

  // Inizializza Ethernet
  
  Ethernet.begin(mac, ip); 

  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

  // Avvia il server
  server.begin();

  T1.begin(MAX31865_4WIRE);
  T2.begin(MAX31865_4WIRE);
  T3.begin(MAX31865_4WIRE);
  T4.begin(MAX31865_4WIRE);
  
}

void loop() {
  // Ascolta le richieste dei client
  EthernetClient client = server.available();
  if (client) {
    BackC=0;
    Serial.println("new client");
    Serial.print("iBackC=");
    Serial.println(BackC);
    
    // Attendere se il client ha inviato dati
    while (client.connected()) {
      if (client.available()) {
        String getmystring = ""
        char c = client.read();
        Serial.print(c);
        if ((c == '\n') || (c == '\r')) {
          getmystring="";
          Serial.print("BackC_r_n=");
          Serial.println(BackC);
          BackC = BackC + 1;
          if (BackC==2){
            BackC=0;
            // Invia la risposta HTTP con il codice HTML e CSS
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.print("<!DOCTYPE html>");
            client.print("<html lang=\"it\">");
                           client.print("<head>");
                           client.print("    <meta charset=\"UTF-8\">");
                           client.print("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
                           client.print("    <title>Pagine con Barra Superiore</title>");
                           client.print("    <style>");
                           client.print("        body { margin: 0; padding: 0; font-family: Arial, sans-serif; }");
                           client.print("        .navbar { background-color: #333; overflow: hidden; }");
                           client.print("        .navbar a { float: left; display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none; }");
                           client.print("        .navbar a:hover { background-color: #ddd; color: black; }");
                           client.print("        .content { padding: 16px; }");
                           client.print("    </style>");
                           client.print("</head>");
                           client.print("<body>");
                           client.print("    <div class=\"navbar\">");
                           client.print("        <a href=\"#pagina1\">Pagina 1</a>");
                           client.print("        <a href=\"#pagina2\">Pagina 2</a>");
                           client.print("        <a href=\"#pagina3\">Pagina 3</a>");
                           client.println("        <a href=\"#aggiungiCodice\">Aggiungi Codice</a>");
                           client.println("    </div>");
                           client.println("    <div class=\"content\">");
                           client.println("      <h1>Dati di Temperatura</h1>");
                           client.println("        <ul>");
                           
                           client.println("        <li>Temperatura 1: <span id=\"temp1\">"); 
                           client.print ( tT1 );
                           client.println(" °C</span></li>");

                           client.println("        <li>Temperatura 2: <span id=\"temp2\">"); 
                           client.print ( tT2 );
                           client.println(" °C</span></li>");

                           client.println("        <li>Temperatura 3: <span id=\"temp3\">"); 
                           client.print ( tT3 );
                           client.println(" °C</span></li>");

                           client.println("        <li>Temperatura 4: <span id=\"temp4\">"); 
                           client.print ( tT4 );
                           client.println(" °C</span></li>");
                           
                           client.println("        </ul>");
                           
                           client.println("    </div>");
                           client.println("</body>");
                           client.println("</html>");
                           client.flush();
                           break;
         }
        }else{
          BackC = 0;
          getmystring = getmystring + c;
          //GET /
        }
      }
    }
    // Chiudi la connessione
    client.stop();
    Serial.println("client disconnected");
  }


  switch (MAS) { 
    case 0:
      if (millis() > TimerDeltaReadT ){
        TimerDeltaReadT= millis() + DeltaRead;
        MAS=1;
        Serial.print("IdT=");
        Serial.println(IdT);
      }
      break;
    case 1:
      switch (IdT) {
        case 0:
          T1.readRTD();
          tT1=T1.temperature(RNOMINAL, RREF);
          IdT=IdT+1;
          break;
        case 1:
          T2.readRTD();
          tT2=T2.temperature(RNOMINAL, RREF);
          IdT=IdT+1;
          break;
        case 2:
          T3.readRTD();
          tT3=T3.temperature(RNOMINAL, RREF);
          IdT=IdT+1;
          break;
        case 3:
          T4.readRTD();
          tT4=T4.temperature(RNOMINAL, RREF);
          IdT=0;
          break;
        default:
          IdT=0;          
          break;
      }
      MAS=0;
      break;
    default:
      MAS=0;
      break;
  }
 
}
