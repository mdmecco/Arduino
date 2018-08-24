
#include <SPI.h>
#include <Ethernet.h>
//#include <SD.h>

unsigned long VBool=0;

const unsigned long TRefresh = 3600000;
unsigned long TG02;

String  mdInSt="";
String St="";
boolean currentLineIsBlank = true;
unsigned int D, E, F, G, H;


unsigned long DayTimeI =0;
unsigned long DayTimeO =0;
Global unsigned long DayTimeT =0;

unsigned int I1[]= { 0,0,0,0};
const char D1[]= {13,10,13,10};

//***************************************** Ingressi ********************
//Uso anche 12 boolean (3 per ogni ingresso per la loro gestione)
// da 0 a 11 nell'indice dei booleani globale
unsigned long PT[]={0,0,0,0};
unsigned long PC[]={0,0,0,0};

//***********************************************************************
unsigned int PaSel=0;
unsigned int RSel=0;
unsigned int LSel=0;

//************** Read Analog *********************
unsigned long Analog[]= {0,0,0};
unsigned long NrAnalog=0;
unsigned int AnalogM[]= {0,0,0};
unsigned long AnalogT =0;
#define AnalogRefresh  2000
unsigned long AnalogWeb = 0;
#define AnalogWebRefresh 600000

//****************************Variabili Gestione Porta Polli
//unsigned long POApertura=450; //tempo in secondi
//unsigned long POChiusura=550; //tempo in secondi
unsigned long POApertura=50400; //tempo in secondi
unsigned long POChiusura=79200; //tempo in secondi
unsigned long PTApertura=15000; //tempo in millisecondi
unsigned long PTChiusura=8000;  //tempo in millisecondi
unsigned long PTRA=0; //per gestione scollegamento rele
//unsigned long PTRC=0; //per gestione scollegamento rele chiusura
//bool PoBool[]={false,false,false};
#define PinApri 7
#define PinChiudi 8


int ret=0;
int rem=0;
int red=0;
unsigned long rt;
unsigned long gt;
unsigned long tt;

//lista delle macchine a stati usate per il programma
unsigned int MAS[] = {0, 0, 0, 0, 0};

bool A=true;


void setup() {
   Serial.begin(9600);
  //Inizializza Ethernet
  EtherSetup();
  

  
  tt=4000-millis();
  TG02=millis()+10000;


  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  digitalWrite(3, true);
  digitalWrite(4, true);
  digitalWrite(5, true);
  digitalWrite(6, true);
  digitalWrite(7, true);
  digitalWrite(8, true);
  
  delay(3000);
  NrAnalog +=1;  
  Analog[0] += analogRead(A3);
  Analog[1] += analogRead(A4);
  Analog[2] += analogRead(A5);
  AnalogM[0]=Analog[0]/NrAnalog;
  AnalogM[1]=Analog[2]/NrAnalog;
  AnalogM[2]=Analog[2]/NrAnalog;
}

void loop() {

//******************************************  listen for incoming Ethernet connections:
  listenForEthernetClients();


//************************************************** Read Analog *************************************

  NrAnalog +=1;
  Analog[0] += analogRead(A3);
  Analog[1] += analogRead(A4);
  Analog[2] += analogRead(A5);
  
  if (millis() > AnalogT ){
    AnalogT=millis()+AnalogRefresh;
    AnalogM[0]=Analog[0]/NrAnalog;
    AnalogM[1]=Analog[2]/NrAnalog;
    AnalogM[2]=Analog[2]/NrAnalog;
    //Serial.print("Numero letture:");
    //Serial.println(NrAnalog);
    AnalogM[0]=0;
    AnalogM[1]=0;
    AnalogM[2]=0;
    NrAnalog =0;
  }

if (millis() > AnalogWeb ){
  AnalogWeb=millis()+AnalogWebRefresh;
  StWeb[5]=true;
}

//*************************************** Pulsante in A ***************
  bitWrite(VBool,0,digitalRead(A0));
  ProcBtn(0, 40000);
  bitWrite(VBool,1,digitalRead(A1));
  ProcBtn(1, 20000);
  bitWrite(VBool,2,digitalRead(A2));
  ProcBtn(2, 20000);
// i bit da 0 a 11 vengono usati per la gestione dei 4 ingressi digitali possibili per ora ne ho configurati solo 3
  
  


//********************************************************************************************  
  if (millis()>tt){
    tt=millis()+1000;
    A=!A;
    digitalWrite(3, A);
  }

//*************************************************** Sincronizzazione orario **********************************
  if ((millis() > TG02) && (MAS[0]==0)){
    TG02=millis()+TRefresh;
    MAS[0]=1;
    MAS[1]=0;
  }

//******************************************* Procedura web *******************************************************

//****************************************** Riporto accensione Luce Polli ****************************************
  if ((PaSel==3) && (MAS[0]==0) && (RSel==0)){
    MAS[0]=1;
    MAS[1]=3;
    PaSel=0;
  }

//****************************************** Riporto accensione Luce Fuori *****************************************
  if ((PaSel==4) && (MAS[0]==0) && (RSel==0)){
    MAS[0]=1;
    MAS[1]=4;
    PaSel=0;
  }

//****************************************** LOG su mdmecco *****************************************
  if (MAS[0]==0){
    IdWeb=-1;
    for (int i=0; i <= 5; i++){
      if (StWeb[i]){
        IdWeb=i;
        break;
      }
    }
    if (IdWeb != -1) {
     Serial.print("IdWeb:");
     Serial.println(IdWeb);
      MAS[0]=1;
      //MAS[1]=5;
      //StWeb[IdWeb]=false;
      // 0 -> Luce Fuori
      // 1 -> Luce Nafta
      // 2 -> Luce Polli
      // 3 -> Porta Polli
      // 4 -> Luce Polli Piccola
      Serial.print("mdInSt:");
      if (IdWeb==0){
        mdInSt="GET /ghelfa/RELESET.php?StatoR=" + BoolToString(!bitRead(VBool,8)) + "&FileTS=LUCEP.TXT HTTP/1.1";
      }else if (IdWeb==1){
        mdInSt="GET /ghelfa/RELESET.php?StatoR=" + BoolToString(!bitRead(VBool,9)) + "&FileTS=LUCE_Nafta.TXT HTTP/1.1";
      }else if (IdWeb==2){
        mdInSt="GET /ghelfa/RELESET.php?StatoR=" + BoolToString(!bitRead(VBool,10)) + "&FileTS=LUCE_Polli.TXT HTTP/1.1";
      }else if (IdWeb==3){
        mdInSt="GET /ghelfa/RELESET.php?StatoR=" + BoolToString(bitRead(VBool,12)) + "&FileTS=Porta_Polli.TXT HTTP/1.1";
      }else if (IdWeb==4){
        mdInSt="GET /ghelfa/RELESET.php?StatoR=" + BoolToString(!bitRead(VBool,11)) + "&FileTS=LUCE_PolliPiccola.TXT HTTP/1.1";
      }else if (IdWeb==5){
        mdInSt="GET /ghelfa/log.php?StatoR=0;" ;//+ (AnalogM[0]) + ";" +(AnalogM[0]) + ";" + (AnalogM[0]) + "&FileTS=ADCLOG_Arduino HTTP/1.1";
        mdInSt.concat(AnalogM[0]);
        mdInSt.concat(";");
        mdInSt.concat(AnalogM[1]);
        mdInSt.concat(";");
        mdInSt.concat(AnalogM[2]);
        mdInSt.concat("&FileTS=ADCLOG_Arduino HTTP/1.1");
      }else{
        mdInSt="GET /ghelfa/RELESET.php?StatoR=0&FileTS=ERRORE.TXT HTTP/1.1";
      }
      Serial.println(mdInSt);
    }
  }
  
  if (MAS[0]==1){
    // *********************************************************************************************Processo stato 1
    Serial.println("Provo a connetermi");
    if ((MAS[1]==0)||(MAS[1]==5)) { //********************************************* Query 0
      ret=mdmecco.connect(serverOra, 80);
      rem=1;
    }else if (MAS[1]==2){ //********************************************* Query 2
      rem=1;
    }else if ((MAS[1]==3)||(MAS[1]==4)) { //********************************************* collegamento ETH
      ret=GenWeb.connect(ETH, 80);
      rem=1;
    }   
    if ((ret)&& (rem)) {
      // Make a HTTP request:
      MAS[0]=2;
      rt=millis()+3000;
      if (MAS[1]==0){  //********************************************* Query 0
        //Leggi ora
        mdInSt="";
        Serial.println("chiedi ora");
        mdmecco.flush();
        mdmecco.println("GET /ghelfa/time.php HTTP/1.1");
        mdmecco.println("Host: www.mdmecco.it");
        mdmecco.println("Accept: text/html");
        mdmecco.println("Connection: keep-alive");
        mdmecco.println("");
        D=0;
        H=0;
      }else if (MAS[1]==2) { //********************************************* Query 2
        Serial.println("chiedi immagine");
      }else if (MAS[1]==3){ //********************************************* collegamento ETH
        Serial.println("commuta ETH");
        GenWeb.println("GET /io.cgi?led=2 HTTP/1.1");
        GenWeb.println("Host: 192.168.1.9");
        GenWeb.println("Accept: text/html");
        GenWeb.println("Connection: keep-alive");
        GenWeb.println("Authorization: Basic YWRtaW46cGFzc3dvcmQ=");
        GenWeb.println("");
        D=0;
      }else if (MAS[1]==4){ //********************************************* collegamento ETH
        Serial.println("commuta ETH");
        GenWeb.println("GET /io.cgi?led=3 HTTP/1.1");
        GenWeb.println("Host: 192.168.1.9");
        GenWeb.println("Accept: text/html");
        GenWeb.println("Connection: keep-alive");
        GenWeb.println("Authorization: Basic YWRtaW46cGFzc3dvcmQ=");
        GenWeb.println("");
        D=0;
      }else if (MAS[1]==5){
        //Leggi ora
        Serial.println("Scrivi LOG");
        mdmecco.flush();
        mdmecco.println(mdInSt);
        mdmecco.println("Host: www.mdmecco.it");
        mdmecco.println("Accept: text/html");
        mdmecco.println("Connection: keep-alive");
        mdmecco.println("");
        D=0;
        H=0;
      }
    }else{
      // if you didn't get a connection to the server:
      Serial.println("connection failed");
      MAS[0]=0;
      MAS[1]=0;
      mdInSt="";
    }
  } else if (MAS[0]==2){  // *********************************************************************************************Processo stato 2
    
    if (MAS[1]==0){ //********************************************* Query 0
      if (mdmecco.available()) {
        rt=millis()+3000;
        char c = mdmecco.read();
          Serial.write(c);
          if (D != 4){
            //Serial.write(c);
            if (D1[D]==c) {
              D +=1 ;
            }else {
              D=0;
            }
          }else{
            //Serial.write(c);
            //Serial.println("DIV DA 4 LIN");
            if ((c != 10) && (c !=13)){
              mdInSt.concat(c);
            //Serial.println("Char:");
              //Serial.write(c);
              //Serial.write('-');
            }else{
              H +=1;
              if (H > 2){
                mdmecco.stop();      
                MAS[0]=3;
                Serial.write('Server ora chiuso');
              }
              //Serial.write('+');
            }
          }
      //}else if (MAS[1]==2){ //********************************************* Query 2
      
      //}else if (MAS[1]==3){ //********************************************* collegamento ETH


      //}else if (MAS[1]==4){ //********************************************* collegamento ETH

      //}else if (MAS[1]==5){ //********************************************* collegamento ETH
        
      }
    }
    if (millis() > rt){
      if (MAS[1]==0){
        Serial.println("");
        Serial.println("Server ora chiuso");
        mdmecco.stop();
      }else if (MAS[1]==2){
        Serial.println("Get CAM chiuso");
      }else if (MAS[1]==3){
        GenWeb.stop();
      }else if (MAS[1]==4){
        GenWeb.stop();
      }else if (MAS[1]==5){
        mdmecco.stop();
      }
      MAS[0]=3;
      Serial.println("Close Client");  
    }
  }else if (MAS[0]==3){  // *********************************************************************************************Processo stato 3
    if (MAS[1]==0){ //********************************************* Query 0
      //Serial.println("++++++++++");
      Serial.println("Ricevuto:" + mdInSt);
      DayTimeI=atol(mdInSt.c_str());
      DayTimeO=millis()/1000;
      //Serial.println(DayTimeI);
      //Serial.println("---------");
      mdInSt="";
    }
    MAS[0]=0;
    MAS[1]=0;
  }
  if (millis() > gt) {
    gt=millis()+5000;
    DayTimeT = ((millis()/1000) - DayTimeO +DayTimeI) % 86400;
    Serial.println(DayTimeT);
  }
}

String BoolToString ( bool Bt){
  if (Bt) {
    return "1";
  }else{
    return "0";
  }
  
}

void listenForEthernetClients() {
   // listen for incoming clients
  // listen for incoming clients
  EthernetClient ghelfa = server.available();
  if (ghelfa) {
    String txtMsg="";
    unsigned int FirstLine=0;
    I1[0]= 0;
    I1[1]= 0;
    I1[2]= 0;
    I1[3]= 0;
    
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (ghelfa.connected()) {
      if (ghelfa.available()) {
        char c = ghelfa.read();
        // Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          Serial.println(txtMsg.c_str());          

          
          ghelfa.println("HTTP/1.1 200 OK");
          ghelfa.println("Content-Type: text/html");
          ghelfa.println("Connection: close");  // the connection will be closed after completion of the response
          //ghelfa.println("Refresh: 5");  // refresh the page automatically every 5 sec
          ghelfa.println();
          ghelfa.println("<!DOCTYPE HTML>");
          ghelfa.println("<html>");
          ghelfa.print("analog input 0:");
          ghelfa.print(AnalogM[0]);
          ghelfa.println("<br />");
          ghelfa.print("analog input 1:");
          ghelfa.print(AnalogM[1]);
          ghelfa.println("<br />");
          ghelfa.print("analog input 2:");
          ghelfa.print(AnalogM[2]);
          ghelfa.println("<br />");
          ghelfa.println("</html>");
          Serial.println("Fine Trasmissione");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          FirstLine=FirstLine+1;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
//***************************************************************** Interpetazione delle linee di comando web ***********************************
          if (FirstLine == 0){
            Serial.write(c);
            // Filtro della prima pagina
            //************************************char P1
            if (P1[I1[0]]== c){
              I1[0] += 1;
            }else{
              I1[0]=0;
            }
            if (I1[0]==L1){
              Serial.println("PaSel 1");
              PaSel=1;
                     
            }

            //************************************char P2
            if (P2[I1[1]]== c){
              I1[1] += 1;
            }else{
              I1[1]=0;
            }
            if (I1[1]==L1){
              Serial.println("PaSel 2");
              PaSel=2;              
             }

            //************************************char P3
            if (P3[I1[2]]== c){
              I1[2] += 1;
            }else{
              I1[2]=0;
            }
            if (I1[2]==L2){
              Serial.println("PaSel 3");
              PaSel=3;              
            }


            //************************************char P4
            if (P4[I1[3]]== c){
              I1[3] += 1;
            }else{
              I1[3]=0;
            }
            if (I1[3]==L3){
              Serial.println("PaSel 4");
              PaSel=4;              
            }

//******************************************************************************** Inserire sopra            
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    ghelfa.stop();
    Serial.println("client disconnected");
  }
}

//*************************************** Funzione che processa i pulsanti **********************************
void ProcBtn (int Ib, unsigned long TAct){
  if (bitRead(VBool,Ib)==bitRead(VBool,(Ib+4))){
    PT[Ib]=millis()+100;
  }else{
    if (millis()> PT[Ib]){
      if ((bitRead(VBool,(Ib+4))==true) && (bitRead(VBool,(Ib+4))!=bitRead(VBool,(Ib)))){
        if (bitRead(VBool,Ib+8)==1){
          bitSet(VBool,Ib+8);  
        }else{
          bitClear(VBool,Ib+8);  
        }
        digitalWrite(Ib, bitRead(VBool,(Ib+8)));
        StWeb[Ib]=true;
        if (bitRead(VBool,Ib+8)==false){
          PC[Ib]=millis()+TAct;    
        }
      }
    bitWrite(VBool,Ib+4,bitRead(VBool,Ib));
    }
  }
  if ((millis()> PC[Ib]) && (bitRead(VBool,Ib)==false)){
    bitSet(VBool,Ib+8);
    digitalWrite(Ib, bitRead(VBool,Ib+8));
    StWeb[Ib]=true;
  }
}

