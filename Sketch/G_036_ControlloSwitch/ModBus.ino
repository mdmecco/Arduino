

//*********************  MODBUS DATI **************
char c;
String comando;
byte MBo[100];
byte NrB=0;
unsigned int MBCRC=0;
//*************************************************


//unsigned long Tom1=0;


//Calcolo CRC
unsigned int CRC16(byte *bytes, byte NrElements){
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos <= NrElements; pos++) {
    crc ^= (unsigned int) bytes[pos];          // XOR byte into least sig. byte of crc
  
    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  return crc;  
}


String PrintArrayHEX (byte *bytes, byte NrElements){
  String Pt ="";
                
  for(byte indice = 0; indice <= NrElements; indice++){
    Pt += String( ((char)bytes[indice]), HEX);
  } 
  Pt += "--" + String(NrElements);
  return Pt;    
}


void OutputStatus(byte Nr, byte StatOut){

  MBo[0]=Nr;    //Nr Scheda
  MBo[1]=0x03;  //Comando
  MBo[2]=16;     //nr dati
  MBo[3]=0;
  MBo[4]=1;
  MBo[5]=0;
  MBo[6]=1;
  MBo[7]=0;
  MBo[8]=1;
  MBo[9]=0;
  MBo[10]=1;
  MBo[11]=0;
  MBo[12]=0;
  MBo[13]=0;
  MBo[14]=1;
  MBo[15]=0;
  MBo[16]=1;
  MBo[17]=0;
  MBo[18]=1;
  MBo[19]=0;
  //MBCRC=CRC16(MBo,19);
  MBo[20]=lowByte(MBCRC);
  MBo[21]=highByte(MBCRC);

  Serial.write((char)MBo[0]);
  Serial.write((char)MBo[1]);
  Serial.write((char)MBo[2]);
  Serial.write((char)MBo[3]);
  Serial.write((char)MBo[4]);
  Serial.write((char)MBo[5]);
  Serial.write((char)MBo[6]);
  Serial.write((char)MBo[7]);
  Serial.write((char)MBo[8]);
  Serial.write((char)MBo[9]);
  Serial.write((char)MBo[10]);
  Serial.write((char)MBo[11]);
  Serial.write((char)MBo[12]);
  Serial.write((char)MBo[13]);
  Serial.write((char)MBo[14]);
  Serial.write((char)MBo[15]);
  Serial.write((char)MBo[16]);
  Serial.write((char)MBo[17]);
  Serial.write((char)MBo[18]);
  Serial.write((char)MBo[19]);
  Serial.write((char)MBo[20]);
  Serial.write((char)MBo[21]);

  
}


byte InputStatus (byte Nr){
  byte Bc =0 ;
  byte id=0;
  byte Rt=0;
  byte Tc=0;
  byte i=0;
  unsigned long TT=0;
  MBo[0]=Nr;
  MBo[1]=3;
  MBo[2]=0;
  MBo[3]=0x81;
  MBo[4]=0;
  MBo[5]=8;
  MBCRC=CRC16(MBo,5);
  MBo[6]=lowByte(MBCRC);
  MBo[7]=highByte(MBCRC);

  Serial.write((char)MBo[0]);
  Serial.write((char)MBo[1]);
  Serial.write((char)MBo[2]);
  Serial.write((char)MBo[3]);
  Serial.write((char)MBo[4]);
  Serial.write((char)MBo[5]);
  Serial.write((char)MBo[6]);
  Serial.write((char)MBo[7]);
  TT=millis()+3000;
  i=0;
  Tc=0;
  while ((Bc == 0) && (i<=20)){
    if (Serial.available() >0){
      Tc=Serial.read();
      if ((i <4)||(i>18)) {

      }else{
        if(Tc!=0){
          //Rt = Rt | (2 ^ ((i-3)/2));
          //Rt = ((i-2)/2);
          //Rt=2 ^ ((i-2)/2);
          Rt=Rt | (byte) pow(2,((i-2)/2)-1);
        }
      }
      i++;
    }
    if (millis() > TT){
      Bc=1;
    }
  }

  if (Bc==0){
      while (Serial.available() >0){
        Bc=Serial.read();
      }
      //delay(200);
      //Serial.write((char)Rt );
      return Rt;
  }else{
    return 0;
  }
}
