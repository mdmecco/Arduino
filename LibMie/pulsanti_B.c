//Versione definita il 04/07/2020

#define Tar 80

//#define Tar 2000



//*****************************************    SETUP Iniziale degli ingressi e uscite 
void SetupIn (byte Id) {          // Funzione Setup In
    if (MySIp == iIn[Id].IdBoard){
      if (iIn[Id].IdPinI != 0xFF) {          
          pinMode(iIn[Id].IdPinI, INPUT);
          digitalWrite(iIn[Id].IdPinI, HIGH);  //questa attivazione serve per attivare la resistenza di pull-up
          digitalRead(iIn[Id].IdPinI) ; 
        }
    }
}

void SetupOut (byte Id) {       // Funzione Setup Out        
    if ( MySIp== iOut[Id].IdBoard){
      if (iOut[Id].IdPinO != 0xFF) {
          pinMode(iOut[Id].IdPinO, OUTPUT);
          // Serial.print("IdOut:");
          // Serial.print(Id);
          // Serial.print(" Level:");
          if (iOut[Id].ActOption == 0){
                // Serial.println("LOW");
                digitalWrite(iOut[Id].IdPinO, LOW);
          }else{
                // Serial.println("HIGH");
                digitalWrite(iOut[Id].IdPinO, HIGH);
          }  
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
      bitWrite((iIn[Id].fl),0, !digitalRead(iIn[Id].IdPinI));
  }else{
      bitWrite((iIn[Id].fl),0, digitalRead(iIn[Id].IdPinI));
  }



  if (bitRead(iIn[Id].fl, 0) == true) {     //Controllo se il pulsante si è attivato
    if (bitRead(iIn[Id].fl, 2) == true) {
        if (millis() > iIn[Id].TAct) {                       //controllo se il tempo di Anti rimbalzo è passato
            bitWrite(iIn[Id].fl, 2, false);                   //se è passato allora attivo l'uscita
            if ( MySIp != iOut[iIn[Id].IdOut].IdBoard){
                UDPIp[3]=iOut[iIn[Id].IdOut].IdBoard;
                MUdp.beginPacket(UDPIp, UdpPort);
                MUdp.write("B");
                MUdp.write(iIn[Id].IdOut);
                MUdp.write("E");
                MUdp.endPacket();
                bitWrite(Debug,0,1);
            }else{
                if (MySIp == iOut[iIn[Id].IdOut].IdBoard){
                    bitWrite(iOut[iIn[Id].IdOut].fl,0,1);
                    //bitWrite(Debug,4,1);
                    
                }
            }
        }
    }
  }else{
    iIn[Id].TAct = millis() + Tar;                         //se è cambiato da prima allora aggiorno il tempo di attivazione
    bitWrite(iIn[Id].fl, 2, true);
  }

}


void wOut(byte Id){
    
    if (bitRead(iOut[Id].fl, 1) == 0){
        if (bitRead(iOut[Id].fl, 0) == 1){
            bitWrite(iOut[Id].fl,1,1);
            bitWrite(iOut[Id].fl,0,0);
            iOut[Id].MillFellOff = millis() + iOut[Id].TOn;
        }
    }else{
        if (bitRead(iOut[Id].fl, 0) == 1){
            iOut[Id].MillFellOff=0;
            bitWrite(iOut[Id].fl,1,0);
            bitWrite(iOut[Id].fl,0,0);
        }else{
          if (millis() > iOut[Id].MillFellOff) {
              bitWrite(iOut[Id].fl,1,0);
          }
        }
    }
    
    
    if ((bitRead(iOut[Id].fl, 1)) != (bitRead(iOut[Id].fl, 3))) {
        if (iOut[Id].ActOption){
            digitalWrite(iOut[Id].IdPinO, !bitRead(iOut[Id].fl, 1));
        }else{
            digitalWrite(iOut[Id].IdPinO, bitRead(iOut[Id].fl, 1));
        }
        bitWrite(iOut[Id].fl,3,(bitRead(iOut[Id].fl, 1)));
    }
}

 
void HTMLStatus(){   //Costruzione della tabello con gli stati di input e di output immediati della scheda
    //****************   INPUT  ******************************************    
    client.println(F("<table style=""width:100%"" border=1>"));
    client.print(F("<tr> <th align=""center"" >"));
    client.print(F("INPUT </th> </tr> </table>"));
    
    client.println(F("<table style=""width:100%"" border=1>"));
    client.print(F("<tr>"));
    for(byte x = 0; x < vIn; x++) {
        if (iIn[x].IdBoard == MySIp){        
          if (digitalRead(iIn[x].IdPinI ) ) {          
            if (iIn[x].ActOption){
                client.print(F("<td align=""center""  bgcolor=""F0F0F0"" >"));
            }else{
                client.print(F("<td align=""center""  bgcolor=""F06060"" >"));
            }
          }else{
            if (iIn[x].ActOption){
                client.print(F("<td align=""center""  bgcolor=""F06060"" >"));
            }else{
                client.print(F("<td align=""center""  bgcolor=""F0F0F0"" >"));
            }
          }
          client.print(x);
          client.print(F(" > "));
          client.print(iIn[x].IdOut);
          client.print(F(" ["));
          client.print(iOut[iIn[x].IdOut].IdBoard);
          client.print(F("] "));
          client.print(F("<br>"));
          client.print(iIn[x].IdPinI);
          client.print(F(" > "));
          client.print(iIn[x].Name);
          client.print(F("<br>"));
          client.print(iOut[iIn[x].IdOut].Name);
          client.print(F("</td>"));
        }
    }
    
    client.println(F("</tr>"));
    client.print(F("</table>"));

    //****************   OUTPUT  ******************************************
    client.println(F("<table style=""width:100%"" border=1>"));
    client.print(F("<tr> <th align=""center"" >"));
    client.print(F("OUTPUT </th> </tr> </table>"));
    
    client.println(F("<table style=""width:100%"" border=1>"));
    client.print(F("<tr>"));
    unsigned long trest=0;
    for(byte x = 0; x < vOut; x++) {
        trest=0;    
      if (iOut[x].IdBoard == MySIp){        
        if (digitalRead(iOut[x].IdPinO ) ) {          
          if (iOut[x].ActOption){
            client.print(F("<td align=""center""  bgcolor=""F0F0F0"" >"));
            trest=0;
          }else{
            client.print(F("<td align=""center""  bgcolor=""F01010"" >"));
            if (millis() < iOut[x].MillFellOff) {
                  trest =((iOut[x].MillFellOff - millis() ) / 60000);
            }
          }  
        }else{
          if (!iOut[x].ActOption){
            client.print(F("<td align=""center""  bgcolor=""F0F0F0"" >"));
            trest=0;
          }else{
            client.print(F("<td align=""center""  bgcolor=""F01010"" >"));
            if (millis() < iOut[x].MillFellOff) {
                  trest =((iOut[x].MillFellOff - millis() ) / 60000);
            }
          }  
        }
        client.print(x);
        client.print(F("<br>"));
        client.print(iOut[x].IdPinO);
        client.print(F(" > "));
        client.print(iOut[x].Name);
        client.print(F("<br>"));
        client.print(trest);
        client.print(F("</td>"));
      }
    }
    
    client.println(F("</tr>"));
    client.print(F("</table>"));




}







void SetupChannel(){
// qui ci sono tutti i canali usati dalle varie centraline
// con la configurazione completa

// Scheda nr 16
    //P Rosso
    iIn[0].IdBoard = 16;        // Indirizzo IP della schedaiIn
    iIn[0].IdOut=1;              //Id riferimento Uscita
    iIn[0].fl=0;              //Gestione Anti-rimbalzo
    iIn[0].IdPinI = 16;       //Pin di ingresso 
    iIn[0].TAct=0;            //gestione dell'antirimbalzo
    iIn[0].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[0].Name ="cam Alle";
    SetupIn(0);
    
    //P Verde
    iIn[1].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[1].IdOut=2;              //Id riferimento Uscita
    iIn[1].fl=0;              //Gestione Anti-rimbalzo
    iIn[1].IdPinI = 4;        //Pin di ingresso 
    iIn[1].TAct=0;            //gestione dell'antirimbalzo
    iIn[1].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[1].Name ="cam 2 Daria";
    SetupIn(1);
    
    //P Blue
    iIn[2].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[2].IdOut=3;              //Id riferimento Uscita
    iIn[2].fl=0;              //Gestione Anti-rimbalzo
    iIn[2].IdPinI = 5;        //Pin di ingresso 
    iIn[2].TAct=0;            //gestione dell'antirimbalzo
    iIn[2].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[2].Name ="cam 1 Daria";
    SetupIn(2);
    
    //P Giallo
    iIn[3].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iIn[3].IdOut=0;              //Id riferimento Uscita
    iIn[3].fl=0;              //Gestione Anti-rimbalzo
    iIn[3].IdPinI = 15;       //Pin di ingresso 
    iIn[3].TAct=0;            //gestione dell'antirimbalzo
    iIn[3].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[3].Name ="Paralume Daria";
    SetupIn(3);
    
    iOut[0].IdBoard = 16;        // Indirizzo IP della schedaiOut
    iOut[0].TOn = 60000;        // Tempo di attività    
    iOut[0].MillFellOff = 0;    // millis del momento di attivazione
    iOut[0].IdPinO = 13;      // Id del pin di uscita del segnale
    iOut[0].ActOption=false;    // Serve per avere pin attivi alto o basso
    iOut[0].Name ="paralume Daria";
    SetupOut(0);

// Scheda nr 14
    iIn[4].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[4].IdOut=1;              //Id riferimento Uscita
    iIn[4].fl=0;              //Gestione Anti-rimbalzo
    iIn[4].IdPinI = 23;       //Pin di ingresso 
    iIn[4].TAct=0;            //gestione dell'antirimbalzo
    iIn[4].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[4].Name ="camera Alle";
    SetupIn(4);

    iIn[5].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[5].IdOut=2;              //Id riferimento Uscita
    iIn[5].fl=0;              //Gestione Anti-rimbalzo
    iIn[5].IdPinI = 25;       //Pin di ingresso 
    iIn[5].TAct=0;            //gestione dell'antirimbalzo
    iIn[5].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[5].Name ="camera Daria";
    SetupIn(5);

    iIn[6].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[6].IdOut=4;              //Id riferimento Uscita
    iIn[6].fl=0;              //Gestione Anti-rimbalzo
    iIn[6].IdPinI = 27;       //Pin di ingresso 
    iIn[6].TAct=0;            //gestione dell'antirimbalzo
    iIn[6].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[6].Name ="Antistalla";
    SetupIn(6);

    iIn[7].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[7].IdOut=5;              //Id riferimento Uscita
    iIn[7].fl=0;              //Gestione Anti-rimbalzo
    iIn[7].IdPinI = 29;       //Pin di ingresso 
    iIn[7].TAct=0;            //gestione dell'antirimbalzo
    iIn[7].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[7].Name ="Corridoio stalla";
    SetupIn(7);

    iIn[8].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[8].IdOut=3;              //Id riferimento Uscita
    iIn[8].fl=0;              //Gestione Anti-rimbalzo
    iIn[8].IdPinI = 31;       //Pin di ingresso 
    iIn[8].TAct=0;            //gestione dell'antirimbalzo
    iIn[8].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[8].Name ="camera Daria 2";
    SetupIn(8);

    iIn[9].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[9].IdOut=7;              //Id riferimento Uscita
    iIn[9].fl=0;              //Gestione Anti-rimbalzo
    iIn[9].IdPinI = 33;       //Pin di ingresso 
    iIn[9].TAct=0;            //gestione dell'antirimbalzo
    iIn[9].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[9].Name ="Specchio Daria";
    SetupIn(9);

    iIn[10].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[10].IdOut=8;              //Id riferimento Uscita
    iIn[10].fl=0;              //Gestione Anti-rimbalzo
    iIn[10].IdPinI = 35;       //Pin di ingresso 
    iIn[10].TAct=0;            //gestione dell'antirimbalzo
    iIn[10].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[10].Name ="";
    SetupIn(10);

    iIn[11].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iIn[11].IdOut=9;              //Id riferimento Uscita
    iIn[11].fl=0;              //Gestione Anti-rimbalzo
    iIn[11].IdPinI = 37;       //Pin di ingresso 
    iIn[11].TAct=0;            //gestione dell'antirimbalzo
    iIn[11].ActOption=false;   // Serve per avere pin attivi alto o basso
    iIn[11].Name ="stalla 1";
    SetupIn(11);

    iOut[1].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[1].TOn = 60000;        // Tempo di attività    
    iOut[1].MillFellOff = 0;    // millis del momento di attivazione
    iOut[1].IdPinO = 22;      // Id del pin di uscita del segnale
    iOut[1].ActOption=true;    // Serve per avere pin attivi alto o basso
    iOut[1].Name = "Camera Alle";
    SetupOut(1);
    
    iOut[2].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[2].TOn = 60000;        // Tempo di attività    
    iOut[2].MillFellOff = 0;    // millis del momento di attivazione
    iOut[2].IdPinO = 24;      // Id del pin di uscita del segnale
    iOut[2].ActOption=true;    // Serve per avere pin attivi alto o basso
    iOut[2].Name = "Camera Daria";
    SetupOut(2);

    iOut[3].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[3].TOn = 60000;        // Tempo di attività    
    iOut[3].MillFellOff = 0;    // millis del momento di attivazione
    iOut[3].IdPinO = 26;      // Id del pin di uscita del segnale
    iOut[3].ActOption=true;    // Serve per avere pin attivi alto o basso
    iOut[3].Name = "Camera Daria 2";
    SetupOut(3);

    iOut[4].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[4].TOn = 60000;        // Tempo di attività    
    iOut[4].MillFellOff = 0;    // millis del momento di attivazione
    iOut[4].IdPinO = 28;      // Id del pin di uscita del segnale
    iOut[4].ActOption=true;    // Serve per avere pin attivi alto o basso
    iOut[4].Name = "Antistalla";
    SetupOut(4);

    iOut[5].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[5].TOn = 60000;        // Tempo di attività    
    iOut[5].MillFellOff = 0;    // millis del momento di attivazione
    iOut[5].IdPinO = 30;      // Id del pin di uscita del segnale
    iOut[5].ActOption=true;    // Serve per avere pin attivi alto o basso
    iOut[5].Name = "Corridoio";
    SetupOut(5);
    
    iOut[6].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[6].TOn = 60000;        // Tempo di attività    
    iOut[6].MillFellOff = 0;    // millis del momento di attivazione
    iOut[6].IdPinO = 32;      // Id del pin di uscita del segnale
    iOut[6].ActOption=true;    // Serve per avere pin attivi alto o basso
    iOut[6].Name = "Specchio Daria";
    SetupOut(6);

    iOut[7].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[7].TOn = 60000;        // Tempo di attività    
    iOut[7].MillFellOff = 0;    // millis del momento di attivazione
    iOut[7].IdPinO = 34;      // Id del pin di uscita del segnale
    iOut[7].ActOption=true;    // Serve per avere pin attivi alto o basso
    SetupOut(7);

    iOut[8].IdBoard = 14;        // Indirizzo IP della schedaiOut
    iOut[8].TOn = 60000;        // Tempo di attività    
    iOut[8].MillFellOff = 0;    // millis del momento di attivazione
    iOut[8].IdPinO = 36;      // Id del pin di uscita del segnale
    iOut[8].ActOption=true;    // Serve per avere pin attivi alto o basso
    SetupOut(8);



    ReadTime();   // legge i tempi personalizzati dalla SD

}


void IncomingUDP(){     // Funzione che legge i comandi UDP in arrivo
  LenUDP = MUdp.parsePacket();
  if (LenUDP > 0 ){
    // Serial.print("Incoming UDP:");
    IdL=0xFF;
    MUdp.read(incomingPacket, LenUDP);
    for (int i = 0; i < LenUDP; i++){
      //// Serial.print(incomingPacket[i]);
      if (incomingPacket[i]=='B'){
        i++;
        IdL=incomingPacket[i];
      }
      if (incomingPacket[i]=='E'){
        // fine lettura pacchetto
      }
    }
    // Serial.println(IdL);
    bitWrite(iOut[IdL].fl,0,1);
  }
}
