
struct BInSt{
  byte IdB=0;
  byte StatI=0;
};

struct BOutSt{
  byte IdB=0;
  byte StatO=0;
  byte StatB=0;
};


struct PwOutSt{
  byte IdGlobal=0;
  byte IdAIn=0;
  byte IdAOut=0;
  byte MaskIn=0;
  byte MaskOut=0;
  unsigned long ActiveTime=0;
  unsigned long millisTime=0;
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
  
  i=BIn[0].IdB;
  //BIn[0].StatI=InputStatus(i);
  a=InputStatus(i);

  BIn[0].StatI=a;
  for (i=0 ; i=6 ; i++){
    //p=BIn[byte(PwOut[i].IdAIn)].StatI;
    a=PwOut[i].IdAIn;
    b=BIn[a].StatI;
    r=PwOut[i].MaskIn;
    if (((p) & r) >0 ){
      /*if (BOut[PwOut[i].IdAOut].StatO & PwOut[i].MaskOut > 0) {
        BOut[PwOut[i].IdAOut].StatO =  (BOut[PwOut[i].IdAOut].StatO ^ PwOut[i].MaskOut); //metto a zero
        PwOut[i].millisTime=0;
      }else{
        BOut[PwOut[i].IdAOut].StatO = (BOut[PwOut[i].IdAOut].StatO | PwOut[i].MaskOut); //metto a 1
        PwOut[i].millisTime=millis()+PwOut[i].ActiveTime;
      } 
    
    }
    
    if (millis() > PwOut[i].millisTime){
      BOut[PwOut[i].IdAOut].StatO =  (BOut[PwOut[i].IdAOut].StatO ^ PwOut[i].MaskOut); //metto a zero
    }

    
  }
  //if ((byte (BOut[0].StatO)) != (byte(BOut[0].StatB))){
    // OutputStatus(BOut[0].IdB, BOut[0].StatO);
    // BOut[0].StatB = byte(BOut[0].StatO;)
  //}

  // OutputStatus(BOut[0].IdB, BOut[0].StatO);

*/ 
}
