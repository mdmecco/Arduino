/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


typedef struct {
  byte IdBoard = 0;                 // Indirizzo IP della scheda
  byte fL = 0;                      // Byte di funzionamento     
  unsigned long TOn = 60000;        // Tempo di attività    
  unsigned long MillFellOff = 0;    // millis del momento di attivazione
  unsigned long TAct =0 ;           // Id del pin di uscita del segnale
  byte IdPinI = 0;                  // millis del momento di pressione del pulsante
  byte IdPinO = 0;                  // Id del pin di uscita del segnale
  byte Options=0;                   // Opzioni varie
} SLight;







