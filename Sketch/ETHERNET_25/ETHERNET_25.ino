#include <pulsanti.h>


unsigned long Dt=0;

// \n = 10 Dec 0x0A
// CR LF = 0x0D 0x0A
// quindi prima lo 0D poi lo 0A


#include <SD.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress myDns(192, 168, 1, 1);
IPAddress ip(192, 168, 1, 25);
EthernetServer server(80);
EthernetClient GetTimeC;

#define EachTimeRequest 3600000


char mdmecco[] = "www.mdmecco.it";
unsigned long GetTime=10000;
byte CheckTime =0;
unsigned long TiCheck=millis()+ 25000;
unsigned long TiDelta=3600000;
//unsigned long TiDelta=25000;
unsigned long TiTimeOut=0;
byte TiMAS=0;    
bool TiCR=false;
String TiStr="";
char Tid=0;

long DayTimeS =-1;

unsigned int IdHTTP=0;


#define PPApri D6
unsigned long PPAAR=2000;            //tempo antirimbalzo in millis()
byte PPASP=B00000000;                       //Stato 
unsigned long PPATAT=0;         //Tempo start attivazione
unsigned long PPATOnAct=18000;   //Tempo massimo acceso

unsigned long TApri=54000;  //15:00
unsigned long TChiudi=79200; //22:00


  
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Server my Version:V003");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, myDns);
  SD.begin();
  delay(1000);
  //server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  delay(1000);

  Serial.print("----- Setup gone------");
}


void loop() {
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = false;
    char HeadHTTP[31];
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (IdHTTP<30){
          HeadHTTP[IdHTTP]=c;
          IdHTTP+=1;
          //IdHTTP= IdHTTP % 98;        
        }
        if (c == '\n') {
          IdHTTP=31;
        }
        
        //Serial.write(c);
         if (c == '\n' && currentLineIsBlank) {
            //String MyString = String(HeadHTTP);
            if (strcmp(HeadHTTP,"GET /gino HTTP")==0){
                Serial.println("-------------- Gino");  
                Serial.println(strcmp(HeadHTTP,"GET /gino HTTP"));  
                Serial.println("-------------- Gino");
            }else{
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close");  // the connection will be closed after completion of the response
                client.println("Refresh: 5");  // refresh the page automatically every 5 sec
                client.println();
                client.println("<!DOCTYPE HTML>");
                client.println("<html>");
                client.println("</html>");
            }
            Serial.println(strcmp(HeadHTTP,"GET /gino HTTP"));
            Serial.println("Ricevuto:");
            Serial.println(HeadHTTP);
            Serial.println("----------------------------------");
            Serial.println(IdHTTP);
            IdHTTP=0;
            break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(100);
    client.stop();
    Serial.println("client disconnected");
  }

  //GetTimeWeb();

  if  (millis() >= Dt){
    Dt=millis()+2000;
    Serial.println(DaySec()); 
  }

   if ((millis() > TiCheck) & (TiMAS==0)){
    TiMAS=1;
    TiCheck=millis()+TiDelta;    
  }
 
  if (TiMAS==1){
    GetTimeC.connect(mdmecco, 80);
    TiMAS=2;
    TiCR=false;
    TiStr="";
    TiTimeOut=millis()+5000;
  }else if(TiMAS==2){
    if (GetTimeC.connected()) {
      GetTimeC.println("GET /ghelfa/time.php HTTP/1.1");
      GetTimeC.println("Host: www.mdmecco.it");
      GetTimeC.println("Connection: close");
      GetTimeC.println();
      TiTimeOut=millis()+5000;
      TiMAS=3;
    }
    if (millis()> TiTimeOut) {
      TiCheck=millis()+8000;
      GetTimeC.stop();
      TiMAS=0;
    }
  }else if(TiMAS==3){
    if (GetTimeC.connected()) {
      if (GetTimeC.available()){
        Tid=GetTimeC.read();
        //Serial.print(Tid);
        if (Tid == 0x0A && TiCR) {
          TiTimeOut=millis()+5000;
          TiMAS=4;
          Tid=0;
          TiCR= false;
        }
        if (Tid == 0x0A) {
          TiCR = true;
        } else if (Tid != 0x0D) {
          TiCR = false;
        }
      }
    }else{
      TiCheck=millis()+3000;
      GetTimeC.stop();  
      TiMAS=0;
    }
    if (millis()> TiTimeOut) {
      TiCheck=millis()+3000;
      GetTimeC.stop();
      TiMAS=0;
    }
    //Serial.println("---3");
  }else if(TiMAS==4){
    if (GetTimeC.connected()){
      if (GetTimeC.available()){
        Tid=GetTimeC.read();
        if ((Tid != 10) && (Tid != 13)) {
          TiStr=TiStr+Tid;
          TiTimeOut=millis()+3000;
        }
      }
    }else{
      GetTimeC.stop();  
      TiMAS=0;
      DayTimeS= atol(TiStr.c_str())- ((millis()/1000) % 86400) ;
      Serial.println(TiStr);
      Serial.println(DayTimeS);
      Serial.println("---END1--");
    }
    if (millis()> TiTimeOut) {
      TiCheck=millis()+5000;
      GetTimeC.stop();
      TiMAS=0;
      Serial.println("---TO--");
    }
  }else if(TiMAS==5){
    Serial.println(TiStr);
    GetTimeC.stop();  
    TiMAS=0;
    Serial.println("---END--");
  }

  if (DaySec() != -1){ //Significa che l'orario è Ok
    if ((DaySec() > TApri) && (DaySec() < (TApri+ 2000))){ 
      // Vedo se devo aprire i polli  
      
    }

    if ((DaySec() > TChiudi) && (DaySec() < (TChiudi+ 2000))){
      // Vedo se devo Chiudere i polli
       
    }
    
  }

//ProcBtn

  
}


long DaySec(){
  if (DayTimeS != -1 ){
    return ((millis()/1000)% 86400) + DayTimeS ;  
  }else{
    return -1;
  }
}
