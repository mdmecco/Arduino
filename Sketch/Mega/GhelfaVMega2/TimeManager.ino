#define EachTimeRequest 3600000

unsigned long GetTime=10000;
byte CheckTime =0;


unsigned long DayTimeS =0;


void CheckTimeMain(){
//Gestione timing della sincronia orario ************************************
  if (millis()>GetTime){
    Serial.println("Scatta richiesta ora");
    int TCheck=FunzioneOrarioCheck(0);
    if (TCheck==0){
      CheckTime=1;
      Serial.println("Vai");
      if (FunzioneOrario(0,0)==0){
        GetTime=millis()+EachTimeRequest;  
      }else{
        GetTime=millis()+20000; 
      }
    }else{
      Serial.println("Richiesta ora, Ethernet Busy No Request");
      GetTime=millis()+5000; 
    }
  }
  if (CheckTime==1){
    int TCheck=FunzioneOrarioCheck(1);
    if (TCheck==0){
      CheckTime=0;
    }else if (TCheck==1){
      GetTime=millis()+20000;
      CheckTime=0;
    }
  }
}
//************************************ Termine gestione ora


unsigned long DaySec(){
  return (((millis()/1000) + DayTimeS) % 86400);
}

void TimeSetUp(long ATime){
  DayTimeS=ATime - ((millis()/1000) % 86400);
  Serial.print("******************************* Time:");
  Serial.println(ATime); 
}
