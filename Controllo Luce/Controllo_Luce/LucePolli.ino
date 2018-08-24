//********************************* Gestione Porta Polli *************************************

unsigned int APBool=0;

unsigned long POApertura=50400; //tempo in secondi
unsigned long POChiusura=79200; //tempo in secondi
unsigned long PTApertura=15000; //tempo in millisecondi
unsigned long PTChiusura=8000;  //tempo in millisecondi
unsigned long PTRA=0; //per gestione scollegamento rele



  if (!bitRead(APBool,1) && (DayTimeT > POApertura) && (DayTimeT < POChiusura)){
    bitSet(VBool,12);
    bitSet(VBool,13);
    digitalWrite(PinApri, false);
    StWeb[3]=true;
    PTRA=millis()+PTApertura;
  }
  if ((bitRead(VBool,13)) && (millis()>PTRA)){
    digitalWrite(PinApri, true);
    bitClear(VBool,13);
    PTRA=0;
  }

  if (bitRead(VBool,12) && (DayTimeT > POChiusura)){
    bitClear(VBool,12);
    bitSet(VBool,13);
    digitalWrite(PinChiudi, false);
    StWeb[3]=true;
    PTRA=millis()+PTChiusura;    
  }
  if ((bitRead(VBool,13)) && (millis()>PTRA)){
    digitalWrite(PinChiudi, true);
    bitClear(VBool,13);
    PTRA=0;
  }

