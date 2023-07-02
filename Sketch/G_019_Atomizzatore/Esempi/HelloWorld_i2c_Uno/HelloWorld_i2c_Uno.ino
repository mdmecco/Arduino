#include <Wire.h> 
#include <LiquidCrystal.h>


//#include <A:\libraries\LiquidCrystal_I2C_V121\LiquidCrystal_I2C.h>
/* --------------------------------------------------------------------------
 2^ TEMPLATE
 Scrivo su un display LCD 20 caratteri x 4 linee, con interfaccia seriale I2C 
 "YwRobot Arduino LCM1602 IIC V1", la frase "AVE\nPROF\Sono trascorsi\n%d secondi" 
 SDA è connesso alla pin analogico A4
 SCL è connesso alla pin analogico A5
 -------------------------------------------------------------------------- */
//#include <Wire.h>  // Libreria di sistema - E' richiesta da I2CIO.cpp
//#include <LiquidCrystal_I2C.h> // Libreria LCD I2C
// Imposta i pin usati sul chip I2C per le connessioni 
// con il display LCD: addr, en, rw ,rs ,d4 ,d5 ,d6 ,d7 ,bl ,blpol
// ed inoltre pone l'indirizzo del display a 0x27
LiquidCrystal lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3,0);

void setup()
{
   // Devo inizializzare il display LCD 20 caratteri per 4 linee
  lcd.begin(20, 4); 
   
  // Pulisce l'LCD e pone il cursore all'inizio
  lcd.clear(); 
   
  // posizionarsi nei punti del display richiesti e scrivere
  // le frasi richieste
  lcd.print("AVE");             // Stampo a video il AVE ...
  lcd.setCursor(0, 1);          // 1° colonna - 2° riga
  lcd.print("PROF");            // e prosegue sulla stessa riga
  lcd.setCursor(0, 2);          // 1° colonna - 3° riga
  lcd.print("Sono trascorsi "); // e prosegue sulla stessa riga
  lcd.setCursor(0, 3);          // 1° colonna - 3° riga
  lcd.print("0 secondi ");      // e prosegue sulla stessa riga
}

void loop()
{
   // INSERIRE QUI LE ISTRUZIONI CHE REPUTI NECESSARIE
   // PER L'AGGIORNAMENTO DELLO SCHERMO
  lcd.setCursor(0, 3);       // 1° colonna - 3° riga
  lcd.print(millis()/1000);
  lcd.print(" SECONDI");     // e prosegue sulla stessa riga
}
