
//Versione definita il 04/07/2020

#define Tar 150

//#define Tar 2000



//*****************************************    SETUP Iniziale degli ingressi e uscite 
void SetupIn (byte Id) {          // Funzione Setup In
    if (iOut[Id].IdBoard == MySIp){
      if (iIn[Id].IdPinI != 0xFF) {          
          pinMode(iIn[Id].IdPinI, INPUT);
          digitalWrite(iIn[Id].IdPinI, HIGH);  //questa attivazione serve per attivare la resistenza di pull-up
        }
    }
}

void SetupOut (byte Id) {       // Funzione Setup Out        
    if (iOut[Id].IdBoard == MySIp){
      if (iOut[Id].IdPinO != 0xFF) {
          pinMode(iOut[Id].IdPinO, OUTPUT);
          digitalWrite(iOut[Id].IdPinO, LOW);  
      }
    }
}


/*

typedef struct {
  byte IdBoard = 0;                 // Indirizzo IP della scheda
  bool Status=false;                // indicatore di stato    
  unsigned long TOn = 60000;        // Tempo di attività    
  unsigned long MillFellOff = 0;    // millis del momento di attivazione
  byte IdPinO = 0xFF;                  // Id del pin di uscita del segnale
  bool ActOption=false;           // Serve per avere pin attivi alto o basso
} AOut;



typedef struct {
    byte IdBoard = 0;                 // Indirizzo IP della scheda
    byte Id=0;                      //Id riferimento uscita
    byte fl=0;                      //Gestione Anti-rimbalzo
    byte IdPinI = 0xFF;             //Pin di ingresso
    unsigned long TAct=0;           //gestione dell'antirimbalzo
    bool ActOption=false;           // Serve per avere pin attivi alto o basso
}   AIn;


*/


void RWIO(byte Id){
  if (iIn[Id].ActOption){
      bitWrite((iIn[Id].fl),0, digitalRead(iIn[Id].IdPinI));
  }else{
      bitWrite((iIn[Id].fl),0, !digitalRead(iIn[Id].IdPinI));
  }

  if (bitRead(iIn[Id].fl, 0) == true) {     //Controllo se il pulsante è nello stato precedente
    if (bitRead(iIn[Id].fl, 2) == false) {
        if (millis() > iIn[Id].TAct) {                       //controllo se il tempo di Anti rimbalzo è passato
            bitWrite(iIn[Id].fl, 2, true);                   //se è passato allora attivo l'uscita
            if (iOut[iIn[Id].Id].IdBoard != MySIp){
                MUdp.beginPacket(iOut[iIn[Id].Id].IdBoard, UdpPort);
                MUdp.write("B");
                MUdp.write(iIn[Id].Id);
                MUdp.write("E");
                MUdp.endPacket();
            }else{
                bitWrite(iOut[iIn[Id].Id].fl,0,true);
            }
        }
    }
  }else{
    iIn[Id].TAct = millis() + Tar;                         //se è cambiato da prima allora aggiorno il tempo di attivazione
    bitWrite(iIn[Id].fl, 2, false);
  }

}


void wOut(byte Id){
    
    if (bitRead(iOut[Id].fl, 1) == false){
        if (bitRead(iOut[Id].fl, 0) == true){
            bitWrite(iOut[Id].fl,1,true);
            iOut[Id].MillFellOff=millis() + iOut[Id].TOn;
        }
    }else{
        if (bitRead(iOut[Id].fl, 0) == true){
            bitWrite(iOut[Id].fl,1,false);
        }else{
            if (millis() > iOut[Id].MillFellOff) {
                bitWrite(iOut[Id].fl,1,false);
            }
        }
    }
    
    if ((bitRead(iOut[Id].fl, 1)) != (bitRead(iOut[Id].fl, 2))) {
        if (bitRead(iOut[Id].ActOption, 1)==false){
            digitalWrite(iOut[Id].IdPinO, bitRead(iOut[Id].fl, 1));
        }else{
            digitalWrite(iOut[Id].IdPinO, !bitRead(iOut[Id].fl, 1));
        }
        if (bitRead(iOut[Id].fl, 1)== false) {
            bitWrite(iOut[Id].fl,2,false);
        }else{
            bitWrite(iOut[Id].fl,2,true);
        }
    }
}

 



void SetupChannel(){
// qui ci sono tutti i canali usati dalle varie centraline
// con la configurazione completa

// Scheda nr 16
    //P Rosso
    iIn[0].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[0].Id=3;              //Id riferimento Ingresso
    iIn[0].fl=0;              //Gestione Anti-rimbalzo
    iIn[0].IdPinI = 16;       //Pin di ingresso 
    iIn[0].TAct=0;            //gestione dell'antirimbalzo
    iIn[0].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(0);
    
    //P Verde
    iIn[1].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[1].Id=1;              //Id riferimento Ingresso
    iIn[1].fl=0;              //Gestione Anti-rimbalzo
    iIn[1].IdPinI = 4;        //Pin di ingresso 
    iIn[1].TAct=0;            //gestione dell'antirimbalzo
    iIn[1].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(1);
    
    //P Blue
    iIn[2].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[2].Id=2;              //Id riferimento Ingresso
    iIn[2].fl=0;              //Gestione Anti-rimbalzo
    iIn[2].IdPinI = 0;        //Pin di ingresso 
    iIn[2].TAct=0;            //gestione dell'antirimbalzo
    iIn[2].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(2);
    
    //P Giallo
    iIn[3].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[3].Id=0;              //Id riferimento Ingresso
    iIn[3].fl=0;              //Gestione Anti-rimbalzo
    iIn[3].IdPinI = 15;       //Pin di ingresso 
    iIn[3].TAct=0;            //gestione dell'antirimbalzo
    iIn[3].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(3);
    
    iOut[0].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iOut[0].TOn = 60000;        // Tempo di attività    
    iOut[0].MillFellOff = 0;    // millis del momento di attivazione
    iOut[0].IdPinO = 13;      // Id del pin di uscita del segnale
    iOut[0].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(0);

// Scheda nr 14
    iIn[4].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[4].Id=2;              //Id riferimento Ingresso
    iIn[4].fl=0;              //Gestione Anti-rimbalzo
    iIn[4].IdPinI = 23;       //Pin di ingresso 
    iIn[4].TAct=0;            //gestione dell'antirimbalzo
    iIn[4].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(4);

    iIn[5].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[5].Id=3;              //Id riferimento Ingresso
    iIn[5].fl=0;              //Gestione Anti-rimbalzo
    iIn[5].IdPinI = 25;       //Pin di ingresso 
    iIn[5].TAct=0;            //gestione dell'antirimbalzo
    iIn[5].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(5);

    iIn[6].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[6].Id=4;              //Id riferimento Ingresso
    iIn[6].fl=0;              //Gestione Anti-rimbalzo
    iIn[6].IdPinI = 27;       //Pin di ingresso 
    iIn[6].TAct=0;            //gestione dell'antirimbalzo
    iIn[6].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(6);

    iIn[7].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[7].Id=5;              //Id riferimento Ingresso
    iIn[7].fl=0;              //Gestione Anti-rimbalzo
    iIn[7].IdPinI = 29;       //Pin di ingresso 
    iIn[7].TAct=0;            //gestione dell'antirimbalzo
    iIn[7].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(7);

    iIn[8].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[8].Id=6;              //Id riferimento Ingresso
    iIn[8].fl=0;              //Gestione Anti-rimbalzo
    iIn[8].IdPinI = 31;       //Pin di ingresso 
    iIn[8].TAct=0;            //gestione dell'antirimbalzo
    iIn[8].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(8);

    iIn[9].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[9].Id=7;              //Id riferimento Ingresso
    iIn[9].fl=0;              //Gestione Anti-rimbalzo
    iIn[9].IdPinI = 33;       //Pin di ingresso 
    iIn[9].TAct=0;            //gestione dell'antirimbalzo
    iIn[9].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(9);

    iIn[10].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[10].Id=8;              //Id riferimento Ingresso
    iIn[10].fl=0;              //Gestione Anti-rimbalzo
    iIn[10].IdPinI = 35;       //Pin di ingresso 
    iIn[10].TAct=0;            //gestione dell'antirimbalzo
    iIn[10].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(10);

    iIn[11].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[11].Id=9;              //Id riferimento Ingresso
    iIn[11].fl=0;              //Gestione Anti-rimbalzo
    iIn[11].IdPinI = 37;       //Pin di ingresso 
    iIn[11].TAct=0;            //gestione dell'antirimbalzo
    iIn[11].ActOption=false;   // Serve per avere pin attivi alto o basso
    SetupIn(11);

    iOut[1].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[1].TOn = 60000;        // Tempo di attività    
    iOut[1].MillFellOff = 0;    // millis del momento di attivazione
    iOut[1].IdPinO = 22;      // Id del pin di uscita del segnale
    iOut[1].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(1);
    
    iOut[2].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[2].TOn = 60000;        // Tempo di attività    
    iOut[2].MillFellOff = 0;    // millis del momento di attivazione
    iOut[2].IdPinO = 24;      // Id del pin di uscita del segnale
    iOut[2].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(2);

    iOut[3].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[3].TOn = 60000;        // Tempo di attività    
    iOut[3].MillFellOff = 0;    // millis del momento di attivazione
    iOut[3].IdPinO = 26;      // Id del pin di uscita del segnale
    iOut[3].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(3);

    iOut[4].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[4].TOn = 60000;        // Tempo di attività    
    iOut[4].MillFellOff = 0;    // millis del momento di attivazione
    iOut[4].IdPinO = 28;      // Id del pin di uscita del segnale
    iOut[4].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(4);

    iOut[5].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[5].TOn = 60000;        // Tempo di attività    
    iOut[5].MillFellOff = 0;    // millis del momento di attivazione
    iOut[5].IdPinO = 30;      // Id del pin di uscita del segnale
    iOut[5].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(5);
    
    iOut[6].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[6].TOn = 60000;        // Tempo di attività    
    iOut[6].MillFellOff = 0;    // millis del momento di attivazione
    iOut[6].IdPinO = 32;      // Id del pin di uscita del segnale
    iOut[6].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(6);

    iOut[7].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[7].TOn = 60000;        // Tempo di attività    
    iOut[7].MillFellOff = 0;    // millis del momento di attivazione
    iOut[7].IdPinO = 34;      // Id del pin di uscita del segnale
    iOut[7].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(7);

    iOut[8].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[8].TOn = 60000;        // Tempo di attività    
    iOut[8].MillFellOff = 0;    // millis del momento di attivazione
    iOut[8].IdPinO = 36;      // Id del pin di uscita del segnale
    iOut[8].ActOption=false;    // Serve per avere pin attivi alto o basso
    SetupOut(8);
   

}


void IncomingUDP(){     // Funzione che legge i comandi UDP in arrivo
  LenUDP = MUdp.parsePacket();
  if (LenUDP){
    IdL=0xFF;
    MUdp.read(incomingPacket, LenUDP);
    for (int i = 0; i < LenUDP; i++){
      if (incomingPacket[i]=="B"){
        i++;
        IdL=incomingPacket[i];
      }
      if (incomingPacket[i]=="E"){
        // fine lettura pacchetto
      }
    }
    bitWrite(iOut[IdL].fl,0,true);
  }
}
