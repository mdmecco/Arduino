
#include <SPI.h>
#include <Ethernet.h>

unsigned long time;
unsigned long TG02;
String txtMsg = "";
String FileIn ="";
String mdInSt="";
boolean currentLineIsBlank = true;
unsigned long MAS2T =0;
unsigned int D;

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
IPAddress mdm(31,11,33,162);

IPAddress G02(192,168,1,202);
IPAddress MyDNS(8,8,8,8);


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient mdmecco;
EthernetClient client1;
EthernetClient ghelfa;

int ret=0;
int rem=0;
int red=0;
unsigned long rt;
unsigned long gt;
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
    Ethernet.begin(mac, ip, MyDNS);
//  }
//  // give the Ethernet shield a second to initialize:
  //server.begin();
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
  //listenForEthernetClients();

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
    MAS[1]=0;
    TG02=millis();
    
  }

  if (MAS[0]==1){
      // *********************************************************************************************Processo stato 1
      if (MAS[1]==0){ //********************************************* Query 0
        ret=mdmecco.connect(serverOra, 80);
        //ret=mdmecco.connect(mdm, 80);
        Serial.println("retmdmecco:" + String(ret));
        rem=1;
        
      }else if (MAS[1]==2){ //********************************************* Query 2
        ret=client1.connect(G02, 2002);
        rem=mdmecco.connect(serverOra, 80);
      }
      if ((ret)&& (rem)) {
        // Make a HTTP request:
        MAS[0]=2;
        rt=millis();
        if (MAS[1]==0){  //********************************************* Query 0
          //Leggi ora
          Serial.println("chiedi ora");
          mdmecco.println("GET /ghelfa/time.php HTTP/1.1");
          mdmecco.println("Host: www.mdmecco.it");
          mdmecco.println("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
          mdmecco.println("Connection: keep-alive");
          mdmecco.println("");
          //mdmecco.println("GET / HTTP/1.1");
          
        }else if (MAS[1]==2) { //********************************************* Query 2
          //Scrivi camera sul web
          //headers = {"Content-type": "JPEG", "Accept": "text/plain", "CamName":"G01", "FileName": NomeFile, "IMAGEPATH": PathNow}
          mdmecco.println("POST /ghelfa/SaveImage.php HTTP/1.1");
          mdmecco.println("Host: www.mdmecco.it");
          mdmecco.println("CamName: G01");
          mdmecco.println("FILENAME: NomeFileJPG");
          mdmecco.println("IMAGEPATH: 2018_04_01");
          Serial.println("chiedi immagine");
          client1.println("GET /jpg/image.jpg HTTP/1.1");
          client1.println("Authorization: Basic YWRtaW46Ym9yZGVy");
          client1.println("");
          D=0;    
      }
     } else {
        // if you didn't get a connection to the server:
        Serial.println("connection failed");
        MAS[0]=0;
        mdInSt="";
     }
  } else if (MAS[0]==2){
    // *********************************************************************************************Processo stato 2
    
    if (MAS[1]==0){ //********************************************* Query 0
      if (mdmecco.available()) {
        rt=millis();
        char c = mdmecco.read();
        Serial.print(c);
      }
    }else if (MAS[1]==2){ //********************************************* Query 2
      if (client1.available()) {
        rt=millis();
        char c = client1.read();
        if (D<=1){
          Serial.print(c);
          if (c==10){
            D=D+1;
          }
        }else{
          //Serial.print(c);
          mdmecco.print(c);
        }
      }
      if (mdmecco.available()) {
        rt=millis();
        char c = mdmecco.read();
        Serial.print(c);
      }
    }
    if ((millis()-rt)>5000){
      if (MAS[1]==0){
        Serial.println("Server ora chiuso");
        mdmecco.stop();

      }else if (MAS[1]==2){
        Serial.println("Get CAM chiuso");
        client1.stop();
        mdmecco.stop();
      }
      MAS[0]=3;
      Serial.println("Close Client");  
    }
  
  }else if (MAS[0]==3){
    // *********************************************************************************************Processo stato 3
   if (MAS[1]==0){ //********************************************* Query 0
      DayTimeI=atoi(mdInSt.c_str());
      DayTimeO=millis()/1000;
  }
  MAS[0]==0;
  MAS[1]==0;
 }else{
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
  
  if (MAS[2]==0){
    EthernetClient ghelfa = server.available();
    if (ghelfa){
      Serial.println("Got a client");
      MAS[2]=1;    
      currentLineIsBlank=true;
      gt=millis()+2500;
    }
  }else if (MAS[2]==1){
      if (ghelfa.available()) {
        gt=millis()+2500;
        Serial.println("AVAILABLE");
        char c = ghelfa.read();
        Serial.println(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          ghelfa.println("HTTP/1.1 200 OK");
          ghelfa.println("Content-Type: text/html");
          ghelfa.println();
          
          // print the current readings, in HTML format:
          ghelfa.print("Temperature: ");
          ghelfa.print(" degrees C");
          ghelfa.println("<br />");
          ghelfa.print("Pressure: ");
          ghelfa.print(" Pa");
          ghelfa.println("<br />");
          MAS[2]=2;          
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }else{
        if (millis()>gt){
          Serial.println("NON CONNESSO");
          MAS[2]=2;
        }
      }
  }else if (MAS[2]==2){
    Serial.println("MAS=2");
    MAS2T=millis()+1500;
    MAS[2]=3;
  }else if (MAS[2]==3){
    if (millis() > MAS2T){
       Serial.println("MAS STOP");
       ghelfa.stop();
       MAS[2]=0;
    }
  }
}

