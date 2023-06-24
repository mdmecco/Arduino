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




void printFiles(File dir, int numTabs)
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
    Serial.println ("FILES PRINT ******************");
    byte dd =0;
    //char buffer[2];
    unsigned int vv=0;
    for (unsigned int i = 0; i < entry.size(); i++ ){
        dd= entry.read ();
        
        if (vv > 31 ) {
            client.print(F( "<br>"));
            vv=0;
            Serial.println ("a capo");          
        }
        vv=vv+1;
        //itoa (dd,16);
        client.print(" ");
        client.print(String(dd, HEX));
        Serial.print(String(dd, HEX));
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
    Serial.println("WRITE");
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
            Serial.print(i);
            Serial.print("  --  ");
            Serial.println(iOut[i].TOn );
        }
    }
    myFile.close();
    //SD.close();
}

void ReadTime(){
    Serial.println ("************Read Time in");
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
    Serial.println ("************Read Time out");
}



