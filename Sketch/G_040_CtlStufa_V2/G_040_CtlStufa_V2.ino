#include <SPI.h>
#include <Ethernet.h>
#include "A:\Sketch\G_040_CtlStufa_V2\temperatura.h"


#include "A:\Sketch\G_040_CtlStufa_V2\temperatura.c"



byte MAS =0;


#define PStufa      34  //Parte se la temperatura della stufa arriva almeno a 60
#define PGiu        35  //Parte se la temperatura dell'accumulatore alto è sopra i 60 e serve caldo in casa
#define PSu         36  //Parte se la temperatura dell'accumulatore alto è sopra i 60 e serve caldo in casa
#define PForesteria 37  //Parte se la temperatura dell'accumulatore alto è sopra i 60 e serve caldo in casa
#define ValvolaSu   38  //Parte se la temperatura dell'accumulatore alto è più alta di 10° dell'accumulatore dell'accumulatore su
#define PSolare     39  //Parte se la temperatura dei pannelli è superiore a quella dell'accumulatore intermedio

#define sON  0
#define sOFF 1

// Modifica con i dettagli della tua rete
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 40);
EthernetServer server(80);
byte EthMAS=0;
unsigned long EthTimeDisconnect=0;

byte BackC=0;

void setup() {
  Serial.begin(9600);

  // Inizializza Ethernet
  
  //Ethernet.begin(mac, ip); 

  //Serial.print("Server is at ");
  //Serial.println(Ethernet.localIP());

  // Avvia il server
  //server.begin();

  digitalWrite ( 34, sOFF);
  digitalWrite ( 35, sOFF);
  digitalWrite ( 36, sOFF);
  digitalWrite ( 37, sOFF);
  digitalWrite ( 38, sOFF);
  digitalWrite ( 39, sOFF);
  digitalWrite ( 40, sOFF);
  digitalWrite ( 41, sOFF);


  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  

  TCN1.begin(MAX31865_4WIRE); //Stufa
  TCN2.begin(MAX31865_4WIRE); //Accumulatore basso
  TCN3.begin(MAX31865_4WIRE); //Accumulatore medio
  TCN4.begin(MAX31865_4WIRE); //Accumulatore alto
  TCN5.begin(MAX31865_4WIRE); //Accumulatore su
  TCN6.begin(MAX31865_4WIRE); //Pannello solare
  TCN7.begin(MAX31865_4WIRE); //Ritorno termosifoni giu
  TCN8.begin(MAX31865_4WIRE); //Ritorno termosifoni su
  
}

void loop() {
  
  ReadTemp();
  
  if (tT5 > 30) {
    digitalWrite ( 34, sON);
  }else{
    digitalWrite ( 34, sOFF);
  }


 
 
}
