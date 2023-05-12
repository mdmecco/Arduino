/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */




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
    char buffer[2];
    unsigned int vv=0;
    for (unsigned int i = 0; i < entry.size(); i++ ){
        entry.read (dd, 1);
        vv= i << 3;
        if ((vv & 8) > 0 ) {
            client.print(F( "<br>"));
            Serial.println ("a capo");          
        }
        itoa (dd,buffer,16);
        client.print(" ");
        client.print(buffer);
    }
    entry.close();
    ShowFile="";
  }
}


void HTMLFileList(){
    client.print(F("<hr width=100% size=4 color=0000FF>\r\n"));   // Linea Separatrice
    File root = SD.open("/");
    printFiles(root,0);
    root.close();
}


void WriteTime(){
    Serial.println("WRITE");
    File myFile;
    myFile = SD.open("DATATIME.BIN", FILE_WRITE);
    for (int i=0 ; i <= TotalOut; i++){
        if (MySIp == iOut[i].IdBoard){
            myFile.write(iOut[i].TOn );
            Serial.println(i);
        }
    }
    myFile.close();
}

void ReadTime(){
    File myFile;
    if (SD.exists("DATATIME.BIN")) {
      myFile = SD.open("DATATIME.BIN", FILE_READ);
      for (int i=0 ; i <= TotalOut; i++){
          if (MySIp == iOut[i].IdBoard){
              myFile.read(iOut[i].TOn, 4 );
          }
      }
      myFile.close();
    }
}



