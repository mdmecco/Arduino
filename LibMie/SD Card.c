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
    for (uint8_t i = 0; i < numTabs; i++)
    {
      client.print('\t');
    }
    client.println(entry.name());
    client.print(F( "<br>"));
    entry.close();
  }
}


void HTMLFileList(){
    client.print(F("<hr width=100% size=4 color=0000FF>\r\n"));   // Linea Separatrice
    File root = SD.open("/");
    printFiles(root,0);
    



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



