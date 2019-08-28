#include <SPI.h>
#include <WiFiNINA.h>

//#include <SPI.h>


IPAddress myDns(8, 8, 8, 8);
IPAddress ETH(192,168,1,6);


byte ArServerStateMachine=0;
byte ArWebSM=0;
byte ArWebTD=0;
byte ArETHSt=0;


char CMdmecco=0x00;
String InMdmecco="";
unsigned long TOmdmecco=0;
unsigned long TConnection=0;
byte LBmdmecco=0;
unsigned long Lmdmecco;
bool HeaderMdmecco=false;
bool BodyMdmecco=false;
#define TOLimit 8000
#define TConnLimit 15000


unsigned long DelayServer=0;
boolean currentLineIsBlank = true;
String GetCmdWeb="";

//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
#define serverOra "www.mdmecco.it"



// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
WiFiServer server(80);
WiFiClient ArServer;
WiFiClient Arduino;
WiFiClient mdmecco;


// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement


int FunzioneOrario(int TD, int ETHs){
  if (ArWebSM==0){
    Serial.println("************** Seconda funzione ********");
    ArWebTD=TD;
    ArETHSt=ETHs;
    ArWebSM=1;  
    return 0;
  }else{
    return 1;
  }
}

int FunzioneOrarioCheck(byte ToDo){
  if (ArWebSM==0){
    //Stato di riposo, ho finito
    return 0;
  }else if (ArWebSM==100) {
    //Stato di Errore
    if (ToDo==1){
      ArWebSM=0;
    }
    return 1;
  }else{
    // Sono in processo
    return 2;
  }
}


void MainClient(){
  if (ArWebSM==0){
    
  }else if(ArWebSM==1){
    InMdmecco="";
    Lmdmecco=0;
    TOmdmecco=millis()+TOLimit;
    TConnection=millis()+ TConnLimit;
    BodyMdmecco=false;
    if (ArWebTD==0){     //**************Server ora mdmecco
      if (mdmecco.connect(serverOra,80)){
        Serial.println("Chiedo Orario");
        mdmecco.println("GET /ghelfa/time.php HTTP/1.1");
        mdmecco.println("Host: www.mdmecco.it");
        mdmecco.println("Accept: text/html");
        mdmecco.println("Connection: keep-alive");
        mdmecco.println("");
        ArWebSM=2;
        LBmdmecco=0;
        HeaderMdmecco=false;
      }else{
        ArWebSM=100;
        Serial.println("ERRORE Connect");
      }
    }else if (ArWebTD==1){  //**************ETH
      if (mdmecco.connect(ETH,80)){
        
        if (ArETHSt==0){
          Serial.println("Chiedo ETH Stato");
          mdmecco.println("GET /status.xml HTTP/1.1");
        }else{
          Serial.print("Set ETH:");
          String f="GET /io.cgi?led=";
          f.concat(byte(ArETHSt-1));
          f.concat(" HTTP/1.1");
          Serial.println(f);
          mdmecco.println(f);  
        }
        //mdmecco.println("Host: );
        mdmecco.println("Accept: text/html");
        mdmecco.println("Connection: keep-alive");
        mdmecco.println("");
        ArWebSM=2;
        LBmdmecco=0;
        HeaderMdmecco=false;
      }else{
        ArWebSM=100;
        Serial.println("ERRORE Connect");
      }
    }
    
  }else if(ArWebSM==2){
    if (mdmecco.available()) {
      TOmdmecco=millis()+TOLimit;
      CMdmecco=mdmecco.read();
      if ((CMdmecco!=10)&&(CMdmecco!=13)) {
        InMdmecco.concat(CMdmecco);
        if (InMdmecco.endsWith("Success!")) {
          ArWebSM=3;
          
        }

        LBmdmecco =0;
        if (HeaderMdmecco==true){
          if (ArWebTD==0){    // ************************* Server Ora
            if (InMdmecco.length()== Lmdmecco){
              //Serial.print("Get Ora:");
              Serial.println(InMdmecco);
              //TimeSetUp(atol(InMdmecco.c_str()));
              ArWebSM=3;
            }  
          }else if (ArWebTD==1){ //********************* ETH Risposta
            
            BodyMdmecco=true;
          }
        }
      }else{
        LBmdmecco +=1;
        if (HeaderMdmecco==false){
          if (LBmdmecco==2){ // qui ci sono le righe di Header
            Serial.print("Linea Header ricevuto:");
            Serial.println(InMdmecco);
            if (InMdmecco.startsWith("Content-Length:")) {
              int d=InMdmecco.indexOf(":");
              InMdmecco=InMdmecco.substring(d+1);
              Lmdmecco=InMdmecco.toInt()-2;
              //Serial.print("**************************Lunghezza**************");
              //Serial.println(Lmdmecco);
            }
            InMdmecco="";
          }
          if (LBmdmecco==4){
            HeaderMdmecco=true;
            InMdmecco="";
          }
        }
            
        if (BodyMdmecco==true){
          if (ArWebTD==1){  //********************* Get ETH senza lunghezza dichiarata
            
            //Serial.println("***************** nessuna Lunghezza *******");
            
            if (ArETHSt!=0){
              ArWebSM=3;  
            }
            if (InMdmecco.endsWith("</response>")) {
              int d=InMdmecco.indexOf("<led3>");
              int e=InMdmecco.indexOf("</led3>");
              Serial.print("Char String:");
              
              if (InMdmecco.substring(d+6,e)=="1"){
                SetOut(1,1);
                Serial.println("1");\
              }else{
                SetOut(1,0);
                Serial.println("0");
              }
              
              //Serial.println(InMdmecco.substring(d+6,e));
              
              
              //MainStF(1,(InMdmecco.substring(d+6,e)=="1"));
              //MSetDigital(8, (InMdmecco.substring(d+6,d+7)=="1"));
              
              //d=InMdmecco.indexOf("<adc1>");
              //e=InMdmecco.indexOf("</adc1>");
              //Serial.println(InMdmecco.substring(d+6,e)); 
              //char carray[InMdmecco.substring(d+6,e).length() + 1]; 
              //InMdmecco.substring(d+6,e).toCharArray(carray, sizeof(carray));
              //TEValueIn(atoi(carray));
              //Serial.println(InMdmecco);
              ArWebSM=3;  
            }
          }
        }
      }
    }
    if (millis()> TOmdmecco){
      Serial.println("************************************");
      Serial.print("TimeOut:");
      Serial.println(InMdmecco);
      Serial.print("Lunghezza2:");
      Serial.println(InMdmecco.length());
      ArWebSM=5;
    }
  }else if(ArWebSM==3){
   mdmecco.stop();   
   ArWebSM=0;
  }else if(ArWebSM==5){
   mdmecco.stop();   
   ArWebSM=0; 
   Serial.println("Funzione orario Errore");
  }else{
    
  }
  if((ArWebSM==100) && (ArWebSM!=0)){ //questa condizione serve per misurare il tempo di connessione ed eventualmente dare un timeout
    if (millis() > TConnection ){
      ArWebSM=5;    //vado nello stato di chiusura e termine del processo con errore 
      TConnection=millis()+ TConnLimit;
    }
  }
}


void EthernetMainServer(){
  if (ArServerStateMachine==0) {
    // listen for incoming clients
    ArServer = server.available();
    if (ArServer) {
      Serial.println("new ArServer");
      currentLineIsBlank = true;
      ArServerStateMachine=1;
    }else{
      ArServer.stop();
    }
  }else if (ArServerStateMachine==1){
    if (ArServer.connected()) { //ricevo della roba
      //************************************************************************************************************************************************************************************************
      if (ArServer.available()) {
        char c = ArServer.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          ArServer.println("HTTP/1.1 200 OK");
          ArServer.println("Content-Type: text/html");
          ArServer.println("Connection: close");  // the connection will be closed after completion of the response
          ArServer.println("Refresh: 5 ;url=/");  // refresh the page automatically every 5 sec
          ArServer.println();
          ArServer.println("<!DOCTYPE HTML>");
          ArServer.println("<html>");
          ArServer.println("<title>Luci Ghelfa</title>");
          ArServer.println("<body text=\"#000000\">");
          ArServer.println("<p align=\"center\">Ghelfa</p>");
          ArServer.println("<table width=\"100%\" border=\"20\" align=\"center\" cellpadding=\"20\" cellspacing=\"20\">");
          ArServer.print("<tr>  <td width=\"50%\" height=\"108\" align=\"center\" valign=\"middle\" bgcolor=\"#");
          //int d=GetStF(1);
          //if (d==1){
          //  ArServer.print("FF0000");
          //}else{
            ArServer.print("00FF66");
          //}
          ArServer.println("\"><a href=\"SWFUORI\"><font face=\"Verdana\" size=\"+5\" color=\"black\">_______ACCESA_______</font></a></td>  </tr>");
          ArServer.print("<tr>  <td width=\"50%\" height=\"108\" align=\"center\" valign=\"middle\" bgcolor=\"#FFFFFF\"><font face=\"Verdana\" size=\"+5\" color=\"black\">");
          float f=0;
          //GetTEValue(f);
          ArServer.println(String(f));
          ArServer.println("</font></a></td>  </tr>");
          ArServer.println("</table>");
          ArServer.println("</body>");
          ArServer.println("</html>");
          
          // output the value of each analog input pin
          //for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          //  int sensorReading = analogRead(analogChannel);
          //  ArServer.print("analog input ");
          //  ArServer.print(analogChannel);
          //  ArServer.print(" is ");
          //  ArServer.print(sensorReading);
          //  ArServer.println("<br />");
          //}
          //ArServer.println("</html>");
          ArServerStateMachine=2;
          //break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          //Serial.println(GetCmdWeb);

          if (GetCmdWeb.indexOf("/ON1")!=-1) {
            SetOut(1,1);
          }
          if (GetCmdWeb.indexOf("/OFF1")!=-1) {
            SetOut(1,0);
          }
          if (GetCmdWeb.indexOf("/SW1")!=-1) {
            SetOut(1,2);
          }
          if (GetCmdWeb.indexOf("/SWFUORI")!=-1) {
            
          }


          
          GetCmdWeb="";
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          //Serial.print(c);
          GetCmdWeb=GetCmdWeb + c; 
        }
      }else{
        ArServerStateMachine=2;
      }
      //****************************************************************************************************************************************************************
    }
      
  }else if (ArServerStateMachine==2){
      ArServerStateMachine=3;
      DelayServer=millis()+1500; // give the web browser time to receive the data
      Serial.println("Wait Delay");
    
  }else if (ArServerStateMachine==3){
    if (millis() > DelayServer) {
      ArServer.stop();
      Serial.println("ArServer disconnected");
      ArServerStateMachine=4;
    }
  }else if (ArServerStateMachine==4){
    ArServerStateMachine=0;
  }else{
    ArServerStateMachine=0;
    ArServer.stop();
  }
}
