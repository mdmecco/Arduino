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

