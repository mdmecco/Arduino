/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */



#define RREF      430.0
#define RNOMINAL  100.0


byte TeMAS=0;

#include <Adafruit_MAX31865.h>

Adafruit_MAX31865 TCN1 = Adafruit_MAX31865(42);
Adafruit_MAX31865 TCN2 = Adafruit_MAX31865(42);
Adafruit_MAX31865 TCN3 = Adafruit_MAX31865(44);
Adafruit_MAX31865 TCN4 = Adafruit_MAX31865(45);
Adafruit_MAX31865 TCN5 = Adafruit_MAX31865(46);
Adafruit_MAX31865 TCN6 = Adafruit_MAX31865(47);
Adafruit_MAX31865 TCN7 = Adafruit_MAX31865(48);
Adafruit_MAX31865 TCN8 = Adafruit_MAX31865(49);


long Ci=5;    // Gradi di isteresi usati per la definizione della temperatura massima     

long tT1 = 0;
long tT2 = 0;
long tT3 = 0;
long tT4 = 0;
long tT5 = 0;
long tT6 = 0;
long tT7 = 0;
long tT8 = 0;

long TbMax=0; //Temperatura massima raggiunta nel ciclo di riscaldamento
long TbMab=0; //Temperatura massima precedente


byte IdT=0;

unsigned long TimerDeltaReadT=0;
unsigned long DeltaRead=1000;
