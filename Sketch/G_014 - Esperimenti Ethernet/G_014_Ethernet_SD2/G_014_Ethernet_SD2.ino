#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1, 14 };
File htmlFile;
EthernetServer server(80);
File root;

 
void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  if (!SD.begin(4)) { return; }

  Serial.begin(9600);
  while (!Serial);

  //Serial.println("Start.....");
  
}
 

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      client.print('\t');
    }
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      client.print("\t\t");
      client.println(entry.size(), DEC);
    }
    entry.close();
  }
}



void loop()
{
  //Serial.println(".....");
  EthernetClient client = server.available();
  if (client) {
    root = SD.open("/");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
            printDirectory(root, 0);          
          }
        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    delay(1);
    client.stop();
  }
}
