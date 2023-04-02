
//Versione definita il 04/07/2020

#define Tar 150

//#define Tar 2000






void SetupSLight (SLight & DLight) {
    if (DLight.IdPinI != 0xFF) {
        pinMode(DLight.IdPinI, INPUT);
        digitalWrite(DLight.IdPinI, HIGH);  //questa attivazione serve per attivare la resistenza di pull-up
    }
    
    if (DLight.IdPinO != 0xFF) {
        pinMode(DLight.IdPinO, OUTPUT);
        digitalWrite(DLight.IdPinO, LOW);  
    }
}




bool SendBtn(SLight & DLight){
    if (bitRead(DLight.fL, 7) == true){
        return true;
    }else{
        return false;
    }
}






bool IfBtn (SLight & DLight) {

  if (bitRead(DLight.Options, 0)==0){
      bitWrite(DLight.fL,0, digitalRead(DLight.IdPinI));
  }else{
      bitWrite(DLight.fL,0, !digitalRead(DLight.IdPinI));
  }


  if (bitRead(DLight.fL, 0) == true) {     //Controllo se il pulsante è nello stato precedente
    if (bitRead(DLight.fL, 2) == false) {
      if (millis() > DLight.TAct) {                       //controllo se il tempo di Anti rimbalzo è passato
          bitWrite(DLight.fL, 2, true);                   //se è passato allora attivo l'uscita
          return true;
      }
    }
    return false;
  }else{
    DLight.TAct = millis() + Tar;                         //se è cambiato da prima allora aggiorno il tempo di attivazione
    bitWrite(DLight.fL, 2, false);
    return false;
  }

}



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

