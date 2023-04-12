

// funzioni per la gestione della data e ora prese dalla rete internet tramite la wifi
unsigned long DayTimeS = 0;
bool DayTimeB = false;
unsigned long DayTimeR = 0;
unsigned long DayDate=0;



String STime (unsigned long TTime){  // ****************  Converte in stringa orario il numero di secondi dalla mezzanotte, o in genere un tempo in secondi in HH:MM
  unsigned int HH=(TTime / 3600);
  unsigned int MM=(TTime % 3600)/60;
  String Tmp="";
  if (HH<10){
    Tmp="0";
    Tmp.concat(String(HH,DEC));
  }else{
    Tmp=(String(HH,DEC));
  }
  Tmp.concat(":");
  if (MM<10){
    Tmp.concat("0");
    Tmp.concat(String(MM,DEC));
  }else{
    Tmp.concat(String(MM,DEC));
  }
  return Tmp;  
}



unsigned long DaySec() {
  if (DayTimeB) {
    return (((millis() / 1000) + DayTimeS) % 86400);
  } else {
    return 0;
  }
}



