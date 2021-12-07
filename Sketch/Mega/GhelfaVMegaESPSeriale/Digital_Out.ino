
#define RL8 21
#define RL7 22
#define RL6 23
#define RL5 24
#define RL4 25
#define RL3 26
#define RL2 27
#define RL1 28

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
  digitalWrite(RL7, HIGH );
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

/*
void DigitalFlash(){
  if (millis() > FlashT){
    //FlashT=millis()+50; //random (5, 100);
    //St= St *2 ;
    St = ((St & 0x80)?0x01:0x00) | (St << 1);
    //St=random(0,255);
    
    if (St & 1) {
      digitalWrite(RL1, HIGH);
    }
    else {
      digitalWrite(RL1, LOW);
    }

    if (St & 2) {
      digitalWrite(RL2, HIGH);
    }
    else {
      digitalWrite(RL2, LOW);
    }

    if (St & 4) {
      digitalWrite(RL3, HIGH);
    }
    else {
      digitalWrite(RL3, LOW);
    }

    if (St & 8) {
      digitalWrite(RL4, HIGH);
    }
    else {
      digitalWrite(RL4, LOW);
    }

    if (St & 16) {
      digitalWrite(RL5, HIGH);
    }
    else {
      digitalWrite(RL5, LOW);
    }

    if (St & 32) {
      digitalWrite(RL6, HIGH);
    }
    else {
      digitalWrite(RL6, LOW);
    }

    if (St & 64) {
      digitalWrite(RL7, HIGH);
    }
    else {
      digitalWrite(RL7, LOW);
    }

    if (St & 128) {
      digitalWrite(RL8, HIGH);
    }
    else {
      digitalWrite(RL8, LOW);
    }
    
  }

}

*/
