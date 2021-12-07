
#define Version "16_02_2020"
#define WhoIAm  "M1";


String InS = "";
String SValue = "";
unsigned int V1 = 0;
unsigned long V2 = 0;

unsigned long D;
  bool CC=true;


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  delay(2000);
  
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  DigitalSetup();
  InitDigitalIn();
  Serial2.flush();
}

void loop() {
  // put your main code here, to run repeatedly:

 ProcDigitaIn();

  //*********************** scrive seriale ***********************
  //**      <ID-CMD-VALUESS-FF>
  //**      InS.substring(1,3)=="M1"
  //**      InS.substring(4,7)=="CMD"
  //**      InS.substring(8,15)=="VALUESS"
  //**      InS.substring(16,18)=="FF"
  //**      <M1-_ON-_____P1-FF>
  //**      <M1-ACT-01>
  //**      InS.substring(8,10)=="01"
  //**
  //**      Comandi:
  //**      ACT -> come il pulsante
  //**      _ON -> Accende indefinitamente a meno di un pulsante
  //**      OFF -> Spegne
  //**      VER -> Versione programma



// void serialEvent1() {
  if (Serial2.available()) {
    //Serial.println("Ciao");
    //digitalWrite(29, HIGH);

    InS = Serial2.readStringUntil('>');
    //digitalWrite(29, LOW);
    if (InS.substring(1, 3) == "M1") {
      if (InS.substring(4, 7) == "ACT") { // Switch dello stato
        SValue=InS.substring(8, 10);
        V1=SValue.toInt();
        ProcFromNetBtn(V1);
      }
      
      if (InS.substring(4, 7) == "_ON") { // Accende senza limite di tempo.... = interruttore
        SValue=InS.substring(8, 10);
        V1=SValue.toInt();
        ProcFromNet(V1, 1);
      }

      if (InS.substring(4, 7) == "OFF") { // Spegne
        SValue=InS.substring(8, 10);
        V1=SValue.toInt();
        ProcFromNet(V1, 0);
      }

      if (InS.substring(4, 7) == "REQ") { // Stato uscite
        SValue="<M1-REQ-";
        SValue.concat(StatusEXIT());
        SValue.concat(">");
        delay(100);
        Serial2.print(SValue);        
      }

      if (InS.substring(4, 7) == "VER") { // Versione
        SValue="<M1-VER-";
        SValue.concat(Version);
        SValue.concat(">");
        delay(100);
        Serial2.print(SValue);        
      }

      
    }
  }
}
