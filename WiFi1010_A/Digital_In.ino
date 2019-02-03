
#define P1 0
#define P2 1
#define Tar 100

unsigned long TP1=0;
byte BP1=0;

unsigned long TP2=0;
byte BP2=0;


void InitDigitalIn(){
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);
  
}

void SetOut (int IdO , int WtD){
  if (IdO==1){
    if (WtD==0){
      bitWrite(BP1,1,false);
    }else if (WtD==1){
      bitWrite(BP1,1,true);
    }else if (WtD==2){
      bitWrite(BP1,1,!bitRead(BP1,1));
    }
  }
  
}


void ProcDigitaIn(){
    bitWrite(BP1,0,digitalRead(P1));
    ProcBtn(BP1,TP1);
    //MSetDigital(1,bitRead(BP1,1));
    
    bitWrite(BP2,0,digitalRead(P2));
    ProcBtn(BP2,TP2);
    //MSetDigital(2,bitRead(BP2,1));
    //MSetDigital(3,bitRead(BP2,2));

    if (bitRead(BP2,6)==1){
      bitWrite(BP2,6,0);
      Serial.println("******************************** Pulsante");
      FunzioneOrario(1,4);
    }
}

void ProcPButton(int Nb){
    if (Nb==1){
      bitWrite(BP1,1,!bitRead(BP1,1));
      //MSetDigital(1,bitRead(BP1,1));
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
      Serial.println("Reset Tempo");
      TAct=millis()+Tar;
      bitWrite(Bf,4,bitRead(Bf,0));  
    }

    // Gestione della commutazione sequenziale di ingresso:2, bit di uscita:1, bit di gestione:3 e 5
    if (bitRead(Bf,2) != bitRead(Bf,5)){
      Serial.println("Entro nel processo");
      bitWrite(Bf,5,bitRead(Bf,2));
      if (bitRead(Bf,2)==true){
        if (bitRead(Bf,3)==true){
          bitWrite(Bf,3,false);
          bitWrite(Bf,1,!bitRead(Bf,1));
        }
      }else{
        bitWrite(Bf,3,true);
        Serial.println("attvo il 3");
      }
    }
}    
