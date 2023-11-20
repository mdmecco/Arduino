/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

union Data
{
  unsigned long l;
  byte b[4];
};



void CFGWrite(){
    Serial.println("Avvio la scrittura di CONFIG.INI");
    File myFile;
    IPAddress localIP (0,0,0,0);
    if (SD.exists("CONFIG.INI")) {
        SD.remove("CONFIG.INI");
    }
    myFile = SD.open("CONFIG.INI", FILE_WRITE);
    myFile.seek(0);
    myFile.print("IP=");
    #if defined (__AVR_ATmega2560__)
        localIP = Ethernet.localIP ;
        myFile.print (localIP[0]);
        myFile.print (".");
        myFile.print (localIP[1]);
        myFile.print (".");
        myFile.print (localIP[2]);
        myFile.print (".");
        myFile.println (localIP [3]);
        
    
    #else
    
    
    #endif
    
    for (byte i=0; i<vIn; i++ ){
        Serial.println(i);
        myFile.print ("InputID=");
        myFile.print (i);
        myFile.println ("|");
        myFile.print ("Name=");
        myFile.print (iIn[i].Name);
        myFile.println ("|");
        myFile.print ("IdBoard=");
        myFile.print (iIn[i].IdBoard);
        myFile.println ("|");
        myFile.print ("IdOut=");
        myFile.print (iIn[i].IdOut);
        myFile.println ("|");
        myFile.print ("fl=");
        myFile.print (iIn[i].fl);
        myFile.println ("|");
        myFile.print ("IdPinI=");
        myFile.print (iIn[i].IdPinI);
        myFile.println ("|");
        myFile.print ("TAct=");
        myFile.print (iIn[i].TAct);
        myFile.println ("|");
        myFile.print ("TOn=");
        myFile.print (iIn[i].TOn);
        myFile.println ("|");
        myFile.print ("ActOption=");
        myFile.print (iIn[i].ActOption);
        myFile.println ("|");
    }
 
    

    myFile.close();
    
}

/*

bool IPRead(IPAddress IPV){
    
    File myFile;
    if (SD.exists("IP.TXT")) {
        myFile = SD.open("IP.TXT");
        if (myFile) {
            // read from the file until there's nothing else in it:
            while (myFile.available()) {
              myfile.readStringUntil(=)
              Serial.write(myFile.read());

            
            
            
            
            }
        }    
        myFile.close();
    }    
}

*/


void printFilesB(File dir, int numTabs)
{
  while (true)
  {
    File entry =  dir.openNextFile();
    if (! entry)
    {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++){
      client.print('\t');
    }
    
    //<a href="url">Link text or object</a>.
    client.print(F( "<a href=""?SHOWFILES="));
    client.println(entry.name());
    client.print(F( """>"));
    client.println(entry.name());
    client.print(F( "</a><br>"));
    //entry.close();
  }
  client.print(F( "<br>"));
  
  if (ShowFile !=""){
    File entry =  SD.open(ShowFile, FILE_READ);
    // Serial.println ("FILES PRINT ******************");
    byte dd =0;
    
    //char buffer[2];
    unsigned int vv=0;
    for (unsigned int i = 0; i < entry.size(); i++ ){
        dd= entry.read ();
        
        if (vv > 31 ) {
            client.print(F( "<br>"));
            vv=0;
            // Serial.println ("a capo");          
        }
        vv=vv+1;
        //itoa (dd,16);
        client.print(" ");
        client.print(String(dd, HEX));
        
    }
    entry.close();
    ShowFile="";
  }
}

void printFiles(File dir, int numTabs)
{
  dir.rewindDirectory();
  int d=0;
  while (true)
  {
    File entry =  dir.openNextFile();
    if (! entry)
    {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++){
      client.print('\t');
    }
    
    //<a href="url">Link text or object</a>.
    client.print(F( "<a href=""?SHOWFILES="));
    client.println(entry.name());
    client.print(F( """>"));
    client.println(entry.name());
    client.print(F( "</a><br>"));
    
  }
  client.print(F( "<br>"));
  
  if (ShowFile !=""){
    File entry =  SD.open(ShowFile, FILE_READ);
    // Serial.println ("FILES PRINT ******************");
    //String Linea;

    while(entry.available()){
        //Linea= entry.readStringUntil("|");
        d=entry.read();
        if (d='|'){
            client.print ("<br>");
            Serial.println("");
        }else{
            if (d != -1){
                client.print (d);
                Serial.print (d);
            }
        }
        //client.println (Linea);
        //client.print ("<br>");
        //Serial.println(Linea);
    }
    entry.close();
    ShowFile="";
  }
}


void HTMLFileList(){
    client.print(F("<hr width=100% size=4 color=0000FF>\r\n"));   // Linea Separatrice
    File root = SD.open("/", "r");
    printFiles(root,0);
    root.close();
}


void WriteTime(){
    
    Data d;
    // Serial.println("WRITE");
    File myFile;
    SD.remove("DATATIME.BIN");
    myFile = SD.open("DATATIME.BIN", FILE_WRITE);
    myFile.seek(0);
    for (int i=0 ; i <= TotalOut; i++){
        if (MySIp == iOut[i].IdBoard){
            d.l=iOut[i].TOn;
            myFile.write(d.b[0] );
            myFile.write(d.b[1] );
            myFile.write(d.b[2] );
            myFile.write(d.b[3] );
            // Serial.print(i);
            // Serial.print("  --  ");
            // Serial.println(iOut[i].TOn );
        }
    }
    myFile.close();
    //SD.close();
}


void ReadSetup(){
    File myFile;
    if (SD.exists("CONFIG.TXT")) {
        myFile = SD.open("CONFIG.TXT");
        if (myFile) {
            // read from the file until there's nothing else in it:
            while (myFile.available()) {
              Serial.write(myFile.read());

            
            
            
            
            }
        }    
        myFile.close();
    }    
}

void ReadTime(){
    // Serial.println ("************Read Time in");
    
    Data d;
    File myFile;
    if (SD.exists("DATATIME.BIN")) {
      myFile = SD.open("DATATIME.BIN", FILE_READ);
      for (int i=0 ; i <= TotalOut; i++){
          if (MySIp == iOut[i].IdBoard){
              d.b[0]=myFile.read();
              d.b[1]=myFile.read();
              d.b[2]=myFile.read();
              d.b[3]=myFile.read();
              iOut[i].TOn=d.l;
          }
      }
      myFile.close();
    }
    // Serial.println ("************Read Time out");
}



