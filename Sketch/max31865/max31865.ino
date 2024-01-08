#include <SPI.h>
#include <Ethernet.h>

// Modifica con i dettagli della tua rete
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 30);
EthernetServer server(80);

byte BackC=0;

void setup() {
  Serial.begin(9600);

  // Inizializza Ethernet
  
  Ethernet.begin(mac, ip); 

  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

  // Avvia il server
  server.begin();
}

void loop() {
  // Ascolta le richieste dei client
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    
    // Attendere se il client ha inviato dati
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        // Se riceviamo una richiesta HTTP completa, invia la risposta
        if ((c == '\n') || (c == '\r')) {
          if (BackC==3){
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
                           client.println("        <h2>Contenuto qui</h2>");
                           client.println("        <p>Puoi aggiungere del codice in questa parte della pagina.</p>");
                           client.println("    </div>");
                           client.println("</body>");
                           client.println("</html>");
                           break;
         }
         BackC = BackC + 1;
         Serial.println(BackC);
        }else{
         BackC=0;
        }
      }
    }

    // Chiudi la connessione
    delay(1);
    client.stop();
    Serial.println("client disconnected");
  }
}
