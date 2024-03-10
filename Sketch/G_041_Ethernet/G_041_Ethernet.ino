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

byte CPGiu=0;  // Variabile usata per il ciclo di funzionamento della pompa giu


unsigned long STimeLog=0;

void setup() {
  Serial.begin(9600);

  // Inizializza Ethernet
  
  //Ethernet.begin(mac, ip); 

  //Serial.print("Server is at ");
  //Serial.println(Ethernet.localIP());

  // Avvia il server
  //server.begin();

  digitalWrite ( PStufa, sOFF);
  digitalWrite ( PGiu, sOFF);
  digitalWrite ( 36, sOFF);
  digitalWrite ( 37, sOFF);
  digitalWrite ( 38, sOFF);
  digitalWrite ( 39, sOFF);
  digitalWrite ( 40, sOFF);
  digitalWrite ( 41, sOFF);


  pinMode(PStufa, OUTPUT);
  pinMode(PGiu, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  

  TCN1.begin(MAX31865_4WIRE); 
  TCN2.begin(MAX31865_4WIRE); 
  TCN3.begin(MAX31865_4WIRE); 
  TCN4.begin(MAX31865_4WIRE); //Stufa
  TCN5.begin(MAX31865_4WIRE); 
  TCN6.begin(MAX31865_4WIRE); //Accumulatore alto
  TCN7.begin(MAX31865_4WIRE); //Accumulatore basso
  TCN8.begin(MAX31865_4WIRE); 
  
}

void loop() {
  
  ReadTemp();

  // se la temperatura della stufa è maggiore dell'accumulatore alto la pompa parte
  if (tT4 > 50) {
    if (tT4 > tT6) {
      digitalWrite ( PStufa, sON);
    }else{
      if (tT4 < tT6) {
        digitalWrite ( PStufa, sOFF);
      }
    }
  }else{
    if (tT4 < 45) {  
      digitalWrite ( PStufa, sOFF);
    }
  }


  // Gestione della pompa termosifoni
  if (tT7 > 55) {
    if (TbMax > TbMab) {
      digitalWrite ( PGiu, sON);
      TbMab=TbMax;
    }
    if (tT7 < (TbMax-Ci)) {
        digitalWrite ( PGiu, sOFF);
        TbMab=TbMax;
        TbMax=tT7;
    }
  }else{
    digitalWrite ( PGiu, sOFF);
    TbMax=tT7;
    TbMab=tT7;
  }



//Logger
  if (millis() > STimeLog) {
    STimeLog = millis()+ 10000;  

    Serial.print (tT1);
    Serial.print (';');
    Serial.print (tT2);
    Serial.print (';');
    Serial.print (tT3);
    Serial.print (';');
    Serial.print (tT4);
    Serial.print (';');
    Serial.print (tT5);
    Serial.print (';');
    Serial.print (tT6);
    Serial.print (';');
    Serial.print (tT7);
    Serial.print (';');
    Serial.print (tT8);
    Serial.print (';');
    if (digitalRead(PStufa)) {
      Serial.print ("OFF");
    }else{
      Serial.print ("ON");  
    }
    Serial.print (';');
    if (digitalRead(PGiu)) {
      Serial.print ("OFF");
    }else{
      Serial.print ("ON");  
    }
    Serial.println ();
  
  }
 
}
