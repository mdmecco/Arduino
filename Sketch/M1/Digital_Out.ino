
#define RL8 22
#define RL7 23
#define RL6 24
#define RL5 25
#define RL4 26
#define RL3 27
#define RL2 28
#define RL1 29

bool BState=LOW;
byte St=254;

void DigitalSetup() {
  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);
  pinMode(RL3, OUTPUT);
  pinMode(RL4, OUTPUT);
  pinMode(RL5, OUTPUT);
  pinMode(RL6, OUTPUT);
  pinMode(RL7, OUTPUT);
  pinMode(RL8, OUTPUT);

  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, HIGH);
  digitalWrite(RL3, HIGH);
  digitalWrite(RL4, HIGH);
  digitalWrite(RL5, HIGH);
  digitalWrite(RL6, HIGH);
  digitalWrite(RL7, HIGH);
  digitalWrite(RL8, HIGH);
  
}

void MSetDigital ( unsigned int Id, bool Status){
    if (Id==0) {

    }else if (Id==1){
      digitalWrite(RL1, !Status);
    }else if (Id==2){
      digitalWrite(RL2, !Status);
    }else if (Id==3){
      digitalWrite(RL3, !Status);
    }else if (Id==4){
      digitalWrite(RL4, !Status);
    }else if (Id==5){
      digitalWrite(RL5, !Status);
    }else if (Id==6){
      digitalWrite(RL6, !Status);
    }else if (Id==7){
      digitalWrite(RL7, !Status);
    }else if (Id==8){
      digitalWrite(RL8, !Status);
    }
}
