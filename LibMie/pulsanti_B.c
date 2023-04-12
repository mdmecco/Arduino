
//Versione definita il 04/07/2020

#define Tar 150

//#define Tar 2000





//*****************************************    SETUP Iniziale degli ingressi e uscite 
void SetupSLight (SLight & DLight) {        
    if (DLight.IdBoard == MyISP){
      if (DLight.IdPinI != 0xFF) {
          pinMode(DLight.IdPinI, INPUT);
          digitalWrite(DLight.IdPinI, HIGH);  //questa attivazione serve per attivare la resistenza di pull-up
      }
      
      if (DLight.IdPinO != 0xFF) {
          pinMode(DLight.IdPinO, OUTPUT);
          digitalWrite(DLight.IdPinO, LOW);  
      }
    }else{
      if (DLight.LoPinI != 0xFF) {          // Se non è una uscita locale allora imposto l'ingresso del pin locale
          pinMode(DLight.LoPinI, INPUT);
          digitalWrite(DLight.LoPinI, HIGH);  //questa attivazione serve per attivare la resistenza di pull-up
      }
    }
}



// si attiva per trasmettere il comando verso un UDP Remoto
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




void SetupChannel(){


  //canali
  iLight[0].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[0].fL = 0;               // Byte di funzionamento     
  iLight[0].TOn = 60000;          // Tempo di attività    
  iLight[0].MillFellOff = 0;      // millis del momento di attivazione
  iLight[0].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[0].IdPinI = 37;          // (Giallo) Id del pin di uscita del segnale
  iLight[0].IdPinO = 36;          // Id del pin di uscita del segnale
  iLight[0].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[0]);
  
  iLight[1].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[1].fL = 0;               // Byte di funzionamento     
  iLight[1].TOn = 60000;          // Tempo di attività    
  iLight[1].MillFellOff = 0;      // millis del momento di attivazione
  iLight[1].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[1].IdPinI = 35;          // (Giallo) Id del pin di uscita del segnale
  iLight[1].IdPinO = 34;          // Id del pin di uscita del segnale
  iLight[1].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[2]);

  iLight[2].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[2].fL = 0;               // Byte di funzionamento     
  iLight[2].TOn = 60000;          // Tempo di attività    
  iLight[2].MillFellOff = 0;      // millis del momento di attivazione
  iLight[2].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[2].IdPinI = 33;          // (Giallo) Id del pin di uscita del segnale
  iLight[2].IdPinO = 32;          // Id del pin di uscita del segnale
  iLight[2].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[2]);

  iLight[3].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[3].fL = 0;               // Byte di funzionamento     
  iLight[3].TOn = 60000;          // Tempo di attività    
  iLight[3].MillFellOff = 0;      // millis del momento di attivazione
  iLight[3].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[3].IdPinI = 31;          // (Giallo) Id del pin di uscita del segnale
  iLight[3].IdPinO = 30;          // Id del pin di uscita del segnale
  iLight[3].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[3]);

  iLight[4].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[4].fL = 0;               // Byte di funzionamento     
  iLight[4].TOn = 60000;          // Tempo di attività    
  iLight[4].MillFellOff = 0;      // millis del momento di attivazione
  iLight[4].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[4].IdPinI = 29;          // (Giallo) Id del pin di uscita del segnale
  iLight[4].IdPinO = 28;          // Id del pin di uscita del segnale
  iLight[4].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[4]);

  iLight[5].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[5].fL = 0;               // Byte di funzionamento     
  iLight[5].TOn = 60000;          // Tempo di attività    
  iLight[5].MillFellOff = 0;      // millis del momento di attivazione
  iLight[5].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[5].IdPinI = 27;          // (Giallo) Id del pin di uscita del segnale
  iLight[5].IdPinO = 26;          // Id del pin di uscita del segnale
  iLight[5].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[5]);

  iLight[6].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[6].fL = 0;               // Byte di funzionamento     
  iLight[6].TOn = 60000;          // Tempo di attività    
  iLight[6].MillFellOff = 0;      // millis del momento di attivazione
  iLight[6].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[6].IdPinI = 25;          // (Giallo) Id del pin di uscita del segnale
  iLight[6].IdPinO = 24;          // Id del pin di uscita del segnale
  iLight[6].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[6]);

  iLight[7].IdBoard = 14;       // Indirizzo IP della scheda
  iLight[7].fL = 0;               // Byte di funzionamento     
  iLight[7].TOn = 60000;          // Tempo di attività    
  iLight[7].MillFellOff = 0;      // millis del momento di attivazione
  iLight[7].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[7].IdPinI = 23;          // (Giallo) Id del pin di uscita del segnale
  iLight[7].IdPinO = 22;          // Id del pin di uscita del segnale
  iLight[7].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[7]);


  // abat.jour comando Daria 
  iLight[8].IdBoard = 16;       // Indirizzo IP della scheda
  iLight[8].fL = 0;               // Byte di funzionamento     
  iLight[8].TOn = 60000;          // Tempo di attività    
  iLight[8].MillFellOff = 0;      // millis del momento di attivazione
  iLight[8].TAct =0 ;             // millis del momento di pressione del pulsante
  iLight[8].IdPinI = 15;          // (Giallo) Id del pin di uscita del segnale
  iLight[8].IdPinO = 13;          // Id del pin di uscita del segnale
  iLight[8].Options=0;            // Indica il tipo di ingresso ed uscita da usare per i canali attivi alto o basso 
  SetupSLight(iLight[8]);

}

