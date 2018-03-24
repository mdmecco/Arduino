
#include <SPI.h>
#include <Ethernet.h>

unsigned long time;
unsigned long TG02;
String txtMsg = "";
String FileIn ="";
String mdInSt="";

unsigned long DayTimeI =0;
unsigned long DayTimeO =0;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// assign an IP address for the controller:
IPAddress ip(192, 168, 1, 9);




//Come sapere che ore sono in char
char serverOra[] = "www.mdmecco.it";
IPAddress ETH(192,168,1,6);

IPAddress G02(192,168,1,202);


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client1;

int ret=0;
int red=0;
unsigned long rt;
int LD=0;

//lista delle macchine a stati usate per il programma
unsigned int MAS[] = {0, 0, 0, 0, 0};

bool A=true;


void setup() {
  Serial.begin(9600);
  
  // start the Ethernet connection and the server:
  //Ethernet.begin(mac, ip);
  
// start the Ethernet connection:
//  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP");
//    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
//  }
//  // give the Ethernet shield a second to initialize:
  server.begin();
  delay(1000);
  time=20000-millis();
  TG02=0;

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  

//-***************************************************************************
}

void loop() {

  // listen for incoming Ethernet connections:
  listenForEthernetClients();

  if ((millis()-time)>500){
    time=millis();
    //Serial.println("MILLIS");
    LD=LD+1;
    A=!A;

    digitalWrite(3, (LD & 1));
    digitalWrite(4, (LD & 2));
    digitalWrite(5, (LD & 4));
    digitalWrite(6, (LD & 8));
    digitalWrite(7, (LD & 16));
    digitalWrite(8, (LD & 32));
    
    if (A==true){
      digitalWrite(LED_BUILTIN, HIGH);
    }else{
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  
  if ((TG02==0) && (millis()>10000)){
    Serial.println("Sono dentro");
    MAS[0]=1;
    MAS[1]=2;
    TG02=millis();
    
  }

  if (MAS[0]==1){
      if (MAS[1]==2){
        ret=client1.connect(G02, 2002);
      }else{
        ret=client1.connect(serverOra, 80);
        Serial.println("retmdmecco:" + String(ret));
      }
      
      if (ret) {
        // Make a HTTP request:
        MAS[0]=2;
        rt=millis();
        
        if (MAS[1]==0){ 
          client1.println("GET /Ghelfa/time.php HTTP/1.1");
        }else if (MAS[1]==2) {
          Serial.println("chiedi immagine");
          client1.println("GET /jpg/image.jpg HTTP/1.1");
          client1.println("Authorization: Basic YWRtaW46Ym9yZGVy");
        } 
        
        //client1.println("Host: ");
        client1.println("Connection: close");
        client1.println();
      } else {
        // if you didn't get a connection to the server:
        Serial.println("connection failed");
        MAS[0]=0;
        mdInSt="";
      }
  }
  if (MAS[0]==2){
    if (client1.available()) {
      rt=millis();
      char c = client1.read();
      //mdInSt +=c;
      Serial.print(c);      
    }else{
      if ((millis()-rt)>5000){
        Serial.println("");
        client1.stop();
        MAS[0]=3;
        Serial.println("Close Client");  
      }
    }
  }

  if (MAS[0]==3){
   if (MAS[1]==1){
      DayTimeI=atoi(mdInSt.c_str());
      DayTimeO=millis()/1000;
   }
 MAS[0]==0;
 MAS[1]==0;
 }
}


unsigned long DayTime(){
  //Ritorna il tempo del giorno in secondi
  return ((DayTimeI+DayTimeO) and 86400);
}

void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client1 = server.available();
  if (client1) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client1.connected()) {
      if (client1.available()) {
        char c = client1.read();
        Serial.println(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client1.println("HTTP/1.1 200 OK");
          client1.println("Content-Type: text/html");
          client1.println();
          // print the current readings, in HTML format:
          client1.print("Temperature: ");
          client1.print(" degrees C");
          client1.println("<br />");
          client1.print("Pressure: ");
          client1.print(" Pa");
          client1.println("<br />");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client1.stop();
  }
}

//************************************************************************************ Client comunication ************************

void Getmdmecco(String PhpProc) {    
  }

