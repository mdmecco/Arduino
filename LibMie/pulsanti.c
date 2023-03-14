
//Versione definita il 04/07/2020

#define Tar 150

//#define Tar 2000


typedef struct {
  byte IdBoard = 0;                 // Indirizzo IP della scheda
  byte fL = 0;                      // Byte di funzionamento     
  unsigned long TOn = 60000;        // Tempo di attività    
  unsigned long MillFellOff = 0;    // millis del momento di attivazione
  unsigned long TAct =0 ;           // Id del pin di uscita del segnale
  byte IdPinI = 0;                  // millis del momento di pressione del pulsante
  byte IdPinO = 0;                  // Id del pin di uscita del segnale
  byte Options=0;                   // Opzioni varie
} SLight;



void ProcBtn1 (SLight & DLight) {
  // Gestione meccanismo di antirimbalzo bit di ingresso:0, bit di uscita:2, bit di gestione:4
  //bit 0 ingresso                                                      Deve essere attivata dall'ingresso
  //bit 1 uscita                                                        Attiva una uscita con uno stato bistabile
  //bit 2 stato pulsante con antirimbalzo                               Attiva l'uscita come monostabile però senza il rimbalzo
  //bit 3 not used
  //bit 4 stato precedente dinamico
  //bit 5 gestione stato commutazione
  //bit 6 Gestione del comando a pulsante da remoto
  //bit 7 va ad 1 alla attivazione del bit 1 (quindi il pulsante con controllo di stato) solo per 1 ciclo


  if (bitRead(DLight.fL, 0) == true) {     //Controllo se il pulsante è nello stato precedente
    if (bitRead(DLight.fL, 2) == false) {
      if (millis() > DLight.TAct) {                       //controllo se il tempo di Anti rimbalzo è passato
          bitWrite(DLight.fL, 2, true);                   //se è passato allora attivo l'uscita
      }
    }
  }else{
    DLight.TAct = millis() + Tar;                         //se è cambiato da prima allora aggiorno il tempo di attivazione
    bitWrite(DLight.fL, 2, false);
  }

 
  // Attivazione da remoto
  if (bitRead(DLight.fL, 6) == true) {
    bitWrite(DLight.fL, 6, false);
    bitWrite(DLight.fL, 2, true);
  }

  if (bitRead(DLight.fL, 7) == true) {
    bitWrite(DLight.fL, 7, false);
  }
  
  if (bitRead(DLight.fL, 2) == true) {
    if (bitRead(DLight.fL, 4)== false ){
      if (bitRead(DLight.fL, 5)== false ){
        bitWrite(DLight.fL, 1, true);
        bitWrite(DLight.fL, 5, true);
        bitWrite(DLight.fL, 4, true);
        bitWrite(DLight.fL, 7, true);
        if (bitRead(DLight.fL, 1) == true){
            DLight.MillFellOff = millis() + DLight.TOn;
        }else{
            DLight.MillFellOff = 0;
        }
      }else{
        bitWrite(DLight.fL, 1, false);
        bitWrite(DLight.fL, 5, false);
        bitWrite(DLight.fL, 7, true);
        bitWrite(DLight.fL, 4, true);
      }
    }
  }else{ 
    bitWrite(DLight.fL, 4, false);
    if ((bitRead(DLight.fL, 1) == true) && (DLight.MillFellOff != 0) && (millis() > DLight.MillFellOff)) {
      bitWrite(DLight.fL, 1, false);
      bitWrite(DLight.fL, 5, false);
      DLight.MillFellOff = 0;
    }
  }
}


//*************************************** Funzione che processa i pulsanti **********************************
void ProcBtn (byte & Bf, unsigned long & TAct, unsigned long & TOnAct , unsigned long & TOn) {
  //Bf     byte di gestione
  //TAct   tempo per antirimbalzo    //momento di pressione antirimbalzo in millis()
  //TOnAct tempo preimpostato per durata accensione
  //Ton    valore dei millis a cui spegnere la luce

  //bit 0 ingresso                                                      Deve essere attivata dall'ingresso
  //bit 1 uscita                                                        Attiva una uscita con uno stato bistabile
  //bit 2 stato pulsante con antirimbalzo                               Attiva l'uscita come monostabile però senza il rimbalzo
  //bit 3 not used
  //bit 4 stato precedente dinamico
  //bit 5 gestione stato commutazione
  //bit 6 Gestione del comando a pulsante da remoto
  //bit 7 va ad 1 alla attivazione del bit 5 (quindi il pulsante con controllo di stato) solo per 1 ciclo


  // Gestione meccanismo di antirimbalzo bit di ingresso:0, bit di uscita:2, bit di gestione:4
  if (bitRead(Bf, 0) == bitRead(Bf, 4)) {
    if (millis() > TAct) {
      //Serial.println("Cambio Stato");
      bitWrite(Bf, 2, bitRead(Bf, 0));
    }
  } else {
    //Serial.println("Reset Tempo");
    TAct = millis() + Tar;
    bitWrite(Bf, 4, bitRead(Bf, 0));
  }


  // Attivazione da remoto
  if (bitRead(Bf, 6) == true) {
    bitWrite(Bf, 6, false);
    bitWrite(Bf, 2, true);
  }

  if (bitRead(Bf, 7) == true) {
    bitWrite(Bf, 7, false);
  }
  
  // Gestione della commutazione sequenziale di ingresso:2, bit di uscita:1, bit di gestione:3 e 5
  if (bitRead(Bf, 2) != bitRead(Bf, 5)) {
    //Serial.println("Entro nel processo");
    bitWrite(Bf, 5, bitRead(Bf, 2));
    if (bitRead(Bf, 2) == true) {
      if (bitRead(Bf, 3) == true) {
        bitWrite(Bf, 3, false);
        bitWrite(Bf, 1, !bitRead(Bf, 1));
        bitWrite(Bf, 7, true);
        if (bitRead(Bf, 1) == true) {
          
          TOn = millis() + TOnAct;
        } else {
          TOn = 0;
        }
      }
    } else {
      bitWrite(Bf, 3, true);
      //Serial.println("attvo il 3");
    }
  }


  //controllo del tempo massimo di attivazione
  if ((bitRead(Bf, 1) == true) && (TOn != 0) && (millis() > TOn)) {
    bitWrite(Bf, 1, false);
    TOn = 0;
  }
}


void RWIoL(SLight & DLight){
    // funzione di lettura ingresso, processo gestione e scrittura uscita
    if (bitRead(DLight.Options, 0)==0){
        bitWrite(DLight.fL,0, digitalRead(DLight.IdPinI));
    }else{
        bitWrite(DLight.fL,0, !digitalRead(DLight.IdPinI));
    }
    ProcBtn1(DLight);
    if (bitRead(DLight.Options, 1)==0){
        digitalWrite(DLight.IdPinO, bitRead(DLight.fL, 1));
    }else{
        digitalWrite(DLight.IdPinO, !bitRead(DLight.fL, 1));
    }
}

