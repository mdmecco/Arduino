

#define P1 42
#define P2 43
#define P3 44
#define P4 45
#define P5 46
#define P6 47
#define P7 48
#define P8 49

#define Tar 100
#define TDelta 3000

//P1 --> Accende il relé nr 1 per mezzora

unsigned long TP1=0;              //tempo antirimbalzo
byte BP1=B00100000;                       //Stato 
unsigned long TOnActP1=1800000;   //Tempo massimo acceso
//unsigned long TOnActP1=3000;   //Tempo massimo acceso
unsigned long TOnP1=3000;            //Tempo start attivazione

unsigned long TP2=0;              //tempo antirimbalzo
byte BP2=B00100000;                       //Stato 
unsigned long TOnActP2=1800000;   //Tempo massimo acceso
//unsigned long TOnActP2=3000;   //Tempo massimo acceso
unsigned long TOnP2=0;            //Tempo start attivazione

unsigned long TP3=0;              //tempo antirimbalzo
byte BP3=B00100000;                       //Stato 
unsigned long TOnActP3=1800000;   //Tempo massimo acceso
//unsigned long TOnActP3=3000;   //Tempo massimo acceso
unsigned long TOnP3=0;            //Tempo start attivazione

unsigned long TP4=0;              //tempo antirimbalzo
byte BP4=B00100000;                       //Stato 
unsigned long TOnActP4=1800000;   //Tempo massimo acceso
//unsigned long TOnActP4=3000;   //Tempo massimo acceso
unsigned long TOnP4=0;            //Tempo start attivazione

unsigned long TP5=0;              //tempo antirimbalzo
byte BP5=B00100000;                       //Stato 
unsigned long TOnActP5=1800000;   //Tempo massimo acceso
//unsigned long TOnActP5=3000;   //Tempo massimo acceso
unsigned long TOnP5=0;            //Tempo start attivazione

unsigned long TP6=0;              //tempo antirimbalzo
byte BP6=B00100000;                       //Stato 
unsigned long TOnActP6=1800000;   //Tempo massimo acceso
//unsigned long TOnActP6=3000;   //Tempo massimo acceso
unsigned long TOnP6=0;            //Tempo start attivazione

unsigned long TP7=0;              //tempo antirimbalzo
byte BP7=B00100000;                       //Stato 
unsigned long TOnActP7=1800000;   //Tempo massimo acceso
//unsigned long TOnActP7=3000;   //Tempo massimo acceso
unsigned long TOnP7=0;            //Tempo start attivazione

unsigned long TP8=0;              //tempo antirimbalzo
byte BP8=B00100000;                       //Stato 
unsigned long TOnActP8=1800000;   //Tempo massimo acceso
//unsigned long TOnActP8=3000;   //Tempo massimo acceso
unsigned long TOnP8=0;            //Tempo start attivazione


void InitDigitalIn(){
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);
  pinMode(P3, INPUT);
  pinMode(P4, INPUT);
  pinMode(P5, INPUT);
  pinMode(P6, INPUT);
  pinMode(P7, INPUT);
  pinMode(P8, INPUT);
}


void ProcFromNetBtn(byte Id ){
    if (Id==1) bitWrite(BP1,6,1);
    if (Id==2) bitWrite(BP2,6,1);
    if (Id==3) bitWrite(BP3,6,1);
    if (Id==4) bitWrite(BP4,6,1);
    if (Id==5) bitWrite(BP5,6,1);
    if (Id==6) bitWrite(BP6,6,1);
    if (Id==7) bitWrite(BP7,6,1);
    if (Id==8) bitWrite(BP8,6,1);
}

void ProcFromNet(byte Id, bool Status ){
    if (Id==1) bitWrite(BP1,1,Status);
    if (Id==2) bitWrite(BP2,1,Status);
    if (Id==3) bitWrite(BP3,1,Status);
    if (Id==4) bitWrite(BP4,1,Status);
    if (Id==5) bitWrite(BP5,1,Status);
    if (Id==6) bitWrite(BP6,1,Status);
    if (Id==7) bitWrite(BP7,1,Status);
    if (Id==8) bitWrite(BP8,1,Status);
}


String StatusEXIT(){
  String St="";
  St.concat(bitRead(BP1,1));
  St.concat(bitRead(BP2,1));
  St.concat(bitRead(BP3,1));
  St.concat(bitRead(BP4,1));
  St.concat(bitRead(BP5,1));
  St.concat(bitRead(BP6,1));
  St.concat(bitRead(BP7,1));
  St.concat(bitRead(BP8,1));
  return St;
}


void ProcDigitaIn(){
    bitWrite(BP1,0,digitalRead(P1));
    ProcBtn(BP1,TP1,TOnActP1, TOnP1);
    if (bitRead(BP1,1)==1){
      bitWrite(BP1,1,0);
      Serial2.print("<LAM-1>");
    }
    //MSetDigital(1,bitRead(BP1,1));
    
    bitWrite(BP2,0,digitalRead(P2));
    ProcBtn(BP2,TP2,TOnActP2, TOnP2);
    MSetDigital(2,bitRead(BP2,1));
    if (bitRead(BP2,1)==1){
      bitWrite(BP2,1,0);
      Serial2.print("<LAM-2>");
    }


    bitWrite(BP3,0,digitalRead(P3));
    ProcBtn(BP3,TP3,TOnActP3, TOnP3);
    MSetDigital(3,bitRead(BP3,1));
    if (bitRead(BP3,1)==1){
      bitWrite(BP3,1,0);
      Serial2.print("<LAM-3>");
    }

    bitWrite(BP4,0,digitalRead(P4));
    ProcBtn(BP4,TP4,TOnActP4, TOnP4);
    MSetDigital(4,bitRead(BP4,1));
    if (bitRead(BP4,1)==1){
      bitWrite(BP4,1,0);
      Serial2.print("<LAM-4>");
    }

    bitWrite(BP5,0,digitalRead(P5));
    ProcBtn(BP5,TP5,TOnActP5, TOnP5);
    MSetDigital(5,bitRead(BP5,1));
    if (bitRead(BP5,1)==1){
      bitWrite(BP5,1,0);
      Serial2.print("<LAM-5>");
    }
    

    bitWrite(BP6,0,digitalRead(P6));
    ProcBtn(BP6,TP6,TOnActP6, TOnP6);
    MSetDigital(6,bitRead(BP6,1));
    if (bitRead(BP6,1)==1){
      bitWrite(BP6,1,0);
      Serial2.print("<LAM-6>");
    }

    bitWrite(BP7,0,digitalRead(P7));
    ProcBtn(BP7,TP7,TOnActP7, TOnP7);
    MSetDigital(7,bitRead(BP7,1));
    if (bitRead(BP7,1)==1){
      bitWrite(BP7,1,0);
      Serial2.print("<LAM-7>");
    }

    bitWrite(BP8,0,digitalRead(P8));
    ProcBtn(BP8,TP8,TOnActP8, TOnP8);
    MSetDigital(8,bitRead(BP8,1));
    if (bitRead(BP8,1)==1){
      bitWrite(BP8,1,0);
      Serial2.print("<LAM-8>");
    }

}


//*************************************** Funzione che processa i pulsanti **********************************
void ProcBtn (byte & Bf, unsigned long & TAct, unsigned long & TOnAct , unsigned long & TOn){
  //Bf     byte di gestione
  //TAct   tempo per antirimbalzo
  //TOnAct tempo preimpostato per durata accensione
  //Ton    valore dei millis a cui spegnere la luce
  
  //bit 0 ingresso                                                      Deve essere attivata dall'ingresso
  //bit 1 uscita                                                        Attiva una uscita con uno stato bistabile
  //bit 2 stato pulsante con antirimbalzo                               Attiva l'uscita come monostabile però senza il rimbalzo
  //bit 3 stato precedente commutazione
  //bit 4 stato precedente dinamico
  //bit 5 gestione stato commutazione
  //bit 6 Gestione del comando a pulsante da remoto
  //bit 7 echo del bit 6 che serve per memoria di stato
  

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

    // Attivazione da remoto
    if (bitRead(Bf,6)==true){
      bitWrite(Bf,6,false);
      bitWrite(Bf,2,true);
    }

    // Gestione della commutazione sequenziale di ingresso:2, bit di uscita:1, bit di gestione:3 e 5
    if (bitRead(Bf,2) != bitRead(Bf,5)){
      //Serial.println("Entro nel processo");
      bitWrite(Bf,5,bitRead(Bf,2));
      if (bitRead(Bf,2)==true){
        if (bitRead(Bf,3)==true){
          bitWrite(Bf,3,false);
          bitWrite(Bf,1,!bitRead(Bf,1));
          if (bitRead(Bf,1)==true){
            TOn=millis() + TOnAct;
          }else{
            TOn=0;
          }
        }
      }else{
        bitWrite(Bf,3,true);
        //Serial.println("attvo il 3");
      }
    }
    
    //controllo del tempo massimo di attivazione
    if ((bitRead(Bf,1)==true) && (TOn != 0) && (millis() > TOn)){ 
      bitWrite(Bf,1,false);
      TOn=0;
    }
}    
