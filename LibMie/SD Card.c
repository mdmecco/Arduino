/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include "SD.h"



void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      client.print('\t');
    }
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      client.print("\t\t");
      client.println(entry.size(), DEC);
    }
    entry.close();
  }
}


void SaveData (byte Id){
  typedef AOut_Pointer *AOut;  
  
  typedef byte_Pointer *byte;
  
  AOut_Pointer Indirizzo_iOut_Id;
  
  byte_Pointer Indirizzo_comesefosseunarraybyte;
  
 int Size = sizeof(AOut); 
  
 Indirizzo_iOut_Id = ( & iOut[Id]);                                   \\legge l'idirizzo della struttura
  
 Indirizzo_comesefosseunarraybyte = (byte_Pointer) Indirizzo_iOut_Id;  \\questa è la funzione di cast della variabile
   
  
}
