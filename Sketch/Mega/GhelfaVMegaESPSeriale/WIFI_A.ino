

///////please enter your sensitive data in the Secret tab/arduino_secrets.h

#define NMWiFi 3
char cBuffer[1024];
unsigned long iBuffer=0;
int A=1;
unsigned long TOutESP=0;

int NetId =10;
int NetSignal=-1000;

bool ESPConnected=false;


long rssi[] = {0,0,0,0};

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int MASGetConn =0;
unsigned long WTGetConn=0;

//int status = WL_IDLE_STATUS;
//WiFiClient client;

int WFStat=0;
byte MASConnection=0;
  

bool WifiConnection() {
  Serial.write("Starts Serial....\n\r");
  Serial1.begin(9600);
  delay (1000);
  //SendESP("AT+UART_DEF=9600,8,1,0,0",0);
  if (SendESP("AT+CWQAP",0)){
    delay(1000);
    if (SendESP("AT",0)){
      delay(1000);
      if (SendESP("AT+CWMODE=1",0)){
        delay(1000);
        if (SendESP("AT+CWDHCP_CUR=1,0",0)){
          delay(1000);                
          if (SendESP("AT+CIPSTA=\"192.168.1.10\",\"192.168.1.1\",\"255.255.255.0\"",0)){
            delay(2000);
            byte b= GetIdBest();
            Serial.print("b-->");
            Serial.println(b);
            if (b>0){
              if (ConnectBest(b)){
                if (SendESP("AT+CIPMUX=1",0)) {
                  if (SendESP("AT+CIPSERVER=1,1001",0)) {
                    ESPConnected=true;
                    Serial.println("---- STARTED SERVER---");
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
  ESPConnected=false;
}


bool ConnectBest(byte wId){
  Serial.println( "----- Connect to Best" );
  Serial.println(wId);
  if (wId==1) {
    if (SendESP("AT+CWJAP_CUR=\"WGhelfa\",\"Lamborghini\"",0)){
      return true;
      Serial.println( cBuffer );
    }
  }else if (wId==2) {
    if (SendESP("AT+CWJAP_CUR=\"GGhelfa\",\"Supergino\"",0)){
      return true;
      Serial.println( cBuffer );
    }
  }
  return false;
}


byte GetIdBest (){
  Serial.println( "--- GetIdBest" );
  if (SendESP("AT+CWLAP",0)){
    int lv1=GetSignalLevel ("\"WGhelfa\"", 9);
    int lv2=GetSignalLevel ("\"GGhelfa\"", 9);
    int lv3=GetSignalLevel ("\"MassaViaBarbieri1968\"", 22);
          Serial.print("lv1-->");
        Serial.println(lv1);

    
    
    if (lv1 != -500) {
      if (lv2 != -500) {
        if (lv1>lv2){
          return 1;
        }else{
          return 2;
        }
      }else{
        return 1; 
      }
    }else{
      if (lv2 != -500) {
        return 2;
      }else{
        return 0;
      }
    }
  }
  return 0;
}


int GetSignalLevel(char Cchar[], byte Clen){
  long f=CheckStr(0,Cchar, Clen); 
  if (f != -1){ 
    long h=CheckStr(f,",", 1); 
    f=CheckStr(h+2,",", 1); 
    return GetValue(h+1 , (f-h-1));
  }else{
    return -500;
  }
}

int GetValue (long iStart, byte iLen){
  char cValue []={0,0,0,0,0,0,0,0,0,0,0,0,0};
  byte d=0;
  for (byte i = iStart; i < (iStart+iLen+1); i = i + 1) {
    cValue[d]=cBuffer[i];
    d=d+1;
  }
  return atoi(cValue);
}

unsigned long CheckStr (long iStart , char Cchar[], byte Clen){
  byte d=0;
  Clen=Clen-1;
  for (long i = iStart; i < 1024; i = i + 1) {
    if (cBuffer[i] == Cchar[d]) {
      if (d==Clen){
        return (i-Clen);
      }
      d = d + 1 ;  
    }else{
      d=0;
    }
  }
  return -1;
}


bool SendESP(String TXS, byte tEnd) {
  //Serial.println(TXS);
  Serial.print("ToESP-->");
  Serial.print(TXS);
  Serial.println("<--ToESP");
  
  A=1;
  MASConnection=0;
  TOutESP=millis();
  while (A){
    if (MASConnection==0){
      TXS += "\r\n";
      Serial1.flush();
      Serial1.write(TXS.c_str());
      //Serial.print("--->");
      //delay (30);
      for (unsigned long i = 0; i < 512; i = i + 1) {
        cBuffer[i]=0;
      }
      iBuffer=0;
      MASConnection=101;
      
    }else if (MASConnection==101){
      if (Serial1.available() > 0) {
        //TOutESP=millis();
        cBuffer[iBuffer] =(char)Serial1.read();
        //Serial.print(cBuffer[iBuffer]);
        if (iBuffer > 2){
          if (tEnd==0) {
            if ((cBuffer[iBuffer-2]=='\n') && (cBuffer[iBuffer-1]=='O') && (cBuffer[iBuffer]=='K')){
              MASConnection=102;
              Serial.println( cBuffer );
              Serial.print("ToESP-->");
              Serial.println("UUUUUUUUUUU0");
              return true;
            }
          }else if (tEnd==1){
            if ((cBuffer[iBuffer-2]==' ') && (cBuffer[iBuffer-1]=='I') && (cBuffer[iBuffer]=='P')){
              MASConnection=102;
              Serial.println( cBuffer );
              Serial.print("ToESP-->");
              Serial.println("UUUUUUUUUUU1");
              return true;
            }
          }
        }
        if (iBuffer > 3){
          if ((cBuffer[iBuffer-4]=='E') && (cBuffer[iBuffer-3]=='R') && (cBuffer[iBuffer-2]=='R') && (cBuffer[iBuffer-1]=='O') && (cBuffer[iBuffer]=='R')){
            MASConnection=102;
            Serial.println( cBuffer );
            Serial.print("ToESP-->");
            Serial.println("-----------ERROR----------");
            return false;
          }
        }

        iBuffer +=1;
        if (iBuffer > 1000){
            MASConnection=102;
            Serial.println( cBuffer );
            Serial.print("ToESP-->");
            Serial.println("EEEEEEEEEEE");
            return false;
        }
      }
      if (millis() > TOutESP+60000){
           MASConnection=102;
           Serial.println( cBuffer );
           Serial.print("ToESP-->");
           Serial.println("TOTOTOTOTOTOTO");
           return false;
      }
    }else {
      A=0;
    }
  }
}


long GetESP(){
  if (ESPConnected) {
    if (Serial1.available() > 0) {
      cBuffer[iBuffer] =(char)Serial1.read();
      //Serial.print(cBuffer[iBuffer]);
      //if ((cBuffer[iBuffer-1]=='D') && (cBuffer[iBuffer]=='\r')){
      if ((cBuffer[iBuffer-2]=='S') && (cBuffer[iBuffer-1]=='E') && (cBuffer[iBuffer]=='D')){
      //if ((cBuffer[iBuffer]=='\n')){
        Serial.print("ESPGot-->");
        Serial.println(cBuffer);
        iBuffer=0;
        //Serial1.flush();
        return 1;
      }else if ((cBuffer[iBuffer-2]=='a') && (cBuffer[iBuffer-1]=='d') && (cBuffer[iBuffer]=='y')){
        WifiConnection();
      }else{
        iBuffer +=1;
        return -1;
      }
    }
    return -2;
  }
  return -1;
}
