 #define Row1 3 
 #define Row2 2
 #define Row3 1
 #define Row4 0
 
 #define Col1 6
 #define Col2 5
 #define Col3 4
 #define Col4 7

 #define Tar 200


unsigned long TP1=0;
byte BP1=0;
unsigned long TP2=0;
byte BP2=0;
unsigned long TP3=0;
byte BP3=0;
unsigned long TP4=0;
byte BP4=0;
unsigned long TP5=0;
byte BP5=0;
unsigned long TP6=0;
byte BP6=0;
unsigned long TP7=0;
byte BP7=0;
unsigned long TP8=0;
byte BP8=0;
unsigned long TP9=0;
byte BP9=0;
unsigned long TP10=0;
byte BP10=10;
unsigned long TP11=0;
byte BP11=0;
unsigned long TP12=0;
byte BP12=0;
unsigned long TP13=0;
byte BP13=0;
unsigned long TP14=0;
byte BP14=0;
unsigned long TP15=0;
byte BP15=0;
unsigned long TP16=0;
byte BP16=0;





void InitPinKey(){  
  //questi ingressi servono per collegare
  //le righe della tastiera
  //notare l’attivazione delle resistenze di pull-up

  pinMode(Row1, INPUT_PULLUP);
  pinMode(Row2, INPUT_PULLUP);
  pinMode(Row3, INPUT_PULLUP);
  pinMode(Row4, INPUT_PULLUP);
   
  //prima colonna
  pinMode(Col1, OUTPUT);
  digitalWrite(Col1, HIGH);
 
  //seconda colonna
  pinMode(Col2, OUTPUT);
  digitalWrite(Col2, HIGH);  
 
  //terza colonna
  pinMode(Col3, OUTPUT);
  digitalWrite(Col3 , HIGH);

  //Quarta colonna
  pinMode(Col4, OUTPUT);
  digitalWrite(Col4 , HIGH);

}


void GetKey(){
 
  //Seleziono la prima colonna
  digitalWrite(Col1, LOW);
  bitWrite(BP1,0,!digitalRead(Row1));
  bitWrite(BP2,0,!digitalRead(Row2));
  bitWrite(BP3,0,!digitalRead(Row3));
  bitWrite(BP4,0,!digitalRead(Row4));
  digitalWrite(Col1, HIGH);

  digitalWrite(Col2, LOW);
  bitWrite(BP5,0,!digitalRead(Row1));
  bitWrite(BP6,0,!digitalRead(Row2));
  bitWrite(BP7,0,!digitalRead(Row3));
  bitWrite(BP8,0,!digitalRead(Row4));
  digitalWrite(Col2, HIGH);

  digitalWrite(Col3, LOW);
  bitWrite(BP9,0,!digitalRead(Row1));
  bitWrite(BP10,0,!digitalRead(Row2));
  bitWrite(BP11,0,!digitalRead(Row3));
  bitWrite(BP12,0,!digitalRead(Row4));
  digitalWrite(Col3, HIGH);

  digitalWrite(Col4, LOW);
  bitWrite(BP13,0,!digitalRead(Row1));
  bitWrite(BP14,0,!digitalRead(Row2));
  bitWrite(BP15,0,!digitalRead(Row3));
  bitWrite(BP16,0,!digitalRead(Row4));
  digitalWrite(Col4, HIGH);

  ProcBtn(BP1,TP1);
  ProcBtn(BP2,TP2);
  ProcBtn(BP3,TP3);
  ProcBtn(BP4,TP4);
  ProcBtn(BP5,TP5);
  ProcBtn(BP6,TP6);
  ProcBtn(BP7,TP7);
  ProcBtn(BP8,TP8);
  ProcBtn(BP9,TP9);
  ProcBtn(BP10,TP10);
  ProcBtn(BP11,TP11);
  ProcBtn(BP12,TP12);
  ProcBtn(BP13,TP13);
  ProcBtn(BP14,TP14);
  ProcBtn(BP15,TP15);
  ProcBtn(BP16,TP16);

  
  if (bitRead(BP1,6)==1) {
    Serial.println("Premuto BP1");
    SendButtonM("<M1-ACT-01>");
    bitWrite(BP1,6,!bitRead(BP1,6));
  }
  if (bitRead(BP2,6)==1) {
    Serial.println("Premuto BP2");
    SendButtonM("<M1-ACT-02>");
    bitWrite(BP2,6,!bitRead(BP2,6));
  }
  if (bitRead(BP3,6)==1) {
    Serial.println("Premuto BP3");
    SendButtonM("<M1-ACT-03>");
    bitWrite(BP3,6,!bitRead(BP3,6));
  }
  
}

//*************************************** Funzione che processa i pulsanti **********************************
void ProcBtn (byte & Bf, unsigned long & TAct){
  //bit 0 ingresso                                                      Deve essere attivata dall'ingresso
  //bit 1 uscita                                                        Attiva una uscita con uno stato bistabile
  //bit 2 stato pulsante con antirimbalzo                               Attiva l'uscita come monostabile però senza il rimbalzo
  //bit 3 stato precedente commutazione
  //bit 4 stato precedente dinamico
  //bit 5 gestione stato commutazione
  //bit 6 Gestione del pulsante ma con disattivazione dopo lettura (prenota la chiamata se si rilasca)
  //bit 7 echo del bit 6 che serve per memoria di stato
  
    if ((bitRead(Bf,2)==1) && (bitRead(Bf,7)==0) ){
      bitWrite(Bf,6,1);
      bitWrite(Bf,7,1);
    }
    if ((bitRead(Bf,2)==0) && (bitRead(Bf,7)==1) ){
      bitWrite(Bf,7,0);
    }
    
    // Gestione meccanismo di antirimbalzo bit di ingresso:0, bit di uscita:2, bit di gestione:4
    if (bitRead(Bf,0) == bitRead(Bf,4)){
      if (millis()> TAct){
        //Serial.println("Cambio Stato");
        bitWrite(Bf,2,bitRead(Bf,0));
      }
    }else{
      //Serial.println("Reset Tempo");
      TAct=millis()+Tar;
      bitWrite(Bf,4,bitRead(Bf,0));  
    }

    // Gestione della commutazione sequenziale di ingresso:2, bit di uscita:1, bit di gestione:3 e 5
    if (bitRead(Bf,2) != bitRead(Bf,5)){
      //Serial.println("Entro nel processo");
      bitWrite(Bf,5,bitRead(Bf,2));
      if (bitRead(Bf,2)==true){
        if (bitRead(Bf,3)==true){
          bitWrite(Bf,3,false);
          bitWrite(Bf,1,!bitRead(Bf,1));
        }
      }else{
        bitWrite(Bf,3,true);
        //Serial.println("attvo il 3");
      }
    }
}    
