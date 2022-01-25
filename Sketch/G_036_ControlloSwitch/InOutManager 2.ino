
struct BInSt{       // Lista degli ingressi
  byte IdB=0;       // Indirizzo Scheda di ingresso    
  byte StatI=0;     // Stato degli ingressi
};

struct BOutSt{      // Lista delle uscite
  byte IdB=0;       // Indirizzo Scheda di uscita 
  byte StatO=0;     // Stato delle Uscite
  byte StatB=0;
};


struct PwOutSt{     //Relazione tra ingressi ed uscite, 
  byte IdGlobal=0;  //Indice del'utilizzatore
  byte IdAIn=0;     // Indice della lista ingressi
  byte IdAOut=0;    // Indice della lista uscite
  byte MaskIn=0;    // Maschera di ingresso
  byte MaskOut=0;   // Maschera di uscita
  byte NetStat=0;
  unsigned long ActiveTime=0;   // Tempo massimo attivazione, se =0 nessuna limitazione
  unsigned long millisTime=0;   // Memoria del momento di attivazione per misura del tempo massimo
};


BInSt   BIn[1];
BOutSt  BOut[1];
PwOutSt PwOut[8];

void SetUpChannels(){
  // definizione delle schede di ingresso e uscita, se la stessa scheda ha degli in e degli out
  // deve essere inserita in entrambe le liste
  BIn[0].IdB=1;
  BOut[0].IdB=1;

  PwOut[0].IdGlobal=1;
  PwOut[0].IdAIn=0;   
  PwOut[0].IdAOut=0;
  PwOut[0].MaskIn=0x01;
  PwOut[0].MaskOut=0x05;
  PwOut[0].ActiveTime=3000;
  
}


void InOutManager(){
  byte i =0;
  byte a =0;
  byte b =0;
  byte c =0;
  byte d =0;
  byte e =0;
  byte r =0;
  bool f =false;
  
  b=BIn[0].IdB;
  a=InputStatus(b); //leggi lo stato degl ingressi della scheda ad indirizzo i
  delay(300);
  /*
  BIn[0].StatI=a;   //Assegna lo stato degli ingressi al corrispettiva variabile nel cluster
    
  for (i=0 ; i=7 ; i++){
    a=PwOut[i].IdAIn;
    b=BIn[a].StatI;
    r=PwOut[i].MaskIn;
    f= ((b & r) > 0);
    if (f){  //se true Significa che l'ingresso � attivo
        a=PwOut[i].IdAOut;
        b=PwOut[i].MaskOut;
        c=BOut[a].StatO;
        f= ((b & c) > 0); // Controlla se lo stato attuale � attivo
        if (f){           // se true significa che lo stato di uscita si deve portare a false
            d= ~b;        // inverto lo stato della maschera
            e= d & c;     // ottendo lo stato corrente della uscita dl nuovo
            BOut[a].StatO = e; // aggiorno lo stato  dell'oscita 
        }else{      // se false significa che lo stato di uscita si deve portare true e settare il tempo di spegnimento
            e= b | c; //attivo l'uscita        
            BOut[a].StatO = e; // aggiorno lo stato  dell'oscita
            PwOut[i].millisTime = millis() + PwOut[i].ActiveTime;
        }
    }// se � false non faccio nulla, quindi non metto il caso else
    
    if (millis() > PwOut[i].millisTime){    // Significa che � passato il tempo di attivazione
            d= ~b;        // inverto lo stato della maschera
            e= d & c;     // ottendo lo stato corrente della uscita dl nuovo
            BOut[a].StatO = e; // aggiorno lo stato  dell'oscita 
    }
  }
  a=PwOut[i].IdAOut;
  e=BOut[a].StatO; */
  OutputStatus(a, e);
}
