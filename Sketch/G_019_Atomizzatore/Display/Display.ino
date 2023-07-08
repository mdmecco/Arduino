#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip


typedef struct {
    byte IdBoard = 0;                 // Indirizzo IP della scheda
    byte IdOut=0;                      //Id riferimento uscita
    byte fl=0;                            //Gestione Anti-rimbalzo
    byte IdPinI = 0xFF;             //Pin di ingresso
    unsigned long TAct=0;           //gestione dell'antirimbalzo
    bool ActOption=false;           // Serve per avere pin attivi alto o basso
}   AIn;


AIn   iIn[1];



// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

const byte ledPin = 13;
const byte interruptPin = 2;
volatile unsigned long PCounter = 0;

unsigned long TTime=0;
unsigned long Tar=70;

SoftwareSerial ss(3, 4);
TinyGPSPlus gps;

unsigned long TMillis=0;
unsigned long dPCount=0;
double Flow=0;
char FlowC[9] = "000000000";
double Speed =0;
double F_S=0;


void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), InCounter, CHANGE);
  
  lcd.begin(LCD_COLS, LCD_ROWS);
  // Print a message to the LCD.
  lcd.backlight();
  //lcd.setCursor(0, 0);
  //lcd.print("Hello, world!");
  lcd.setCursor( 0,1);
  //lcd.print("It Works!"); 
  lcd.setCursor(0,3);
  //lcd.print("Ciao Vika"); 

  lcd.setCursor(0, 2);
  lcd.print("C:");

    iIn[0].IdBoard = 0;        // Indirizzo IP della schedaiIn
    iIn[0].IdOut=1;              //Id riferimento Uscita
    iIn[0].fl=0;              //Gestione Anti-rimbalzo
    iIn[0].IdPinI = 5;       //Pin di ingresso 
    iIn[0].TAct=70;            //gestione dell'antirimbalzo
    iIn[0].ActOption=false;   // Serve per avere pin attivi alto o basso
  
    //SetupIn(0);
    
    //P Verde
    iIn[1].IdBoard = 0;        // Indirizzo IP della schedaiOut
    iIn[1].IdOut=2;              //Id riferimento Uscita
    iIn[1].fl=0;              //Gestione Anti-rimbalzo
    iIn[1].IdPinI = 6;        //Pin di ingresso 
    iIn[1].TAct=70;            //gestione dell'antirimbalzo
    iIn[1].ActOption=false;   // Serve per avere pin attivi alto o basso
    //SetupIn(1);
  

}

void loop() {

  while (ss.available()){
    gps.encode(ss.read());
    //Serial.write(ss.read());
  }

  if (millis() > TTime) {
    TTime=millis()+1000; 
    lcd.setCursor(2, 2);
    lcd.print(PCounter); 
    lcd.setCursor(0, 0);
    lcd.print(gps.time.hour());
    lcd.print(":"); 
    lcd.print(gps.time.minute());
    lcd.print(":"); 
    lcd.print(gps.time.second());
    Flow=(double(PCounter - dPCount) / double( millis() - TMillis)) *60000;
    dPCount=PCounter;
    TMillis=millis();
       
    lcd.setCursor(0, 1);
    sprintf(FlowC, "%9d", Flow);
    lcd.print(FlowC);
    
    lcd.setCursor(10, 1);
    Speed=gps.speed.kmph();
    sprintf(FlowC, "%9d", int(Speed));
    lcd.print(FlowC);

    lcd.setCursor(10, 3);
    F_S=Flow/Speed;
    sprintf(FlowC, "%9d", int(F_S));
    lcd.print(FlowC);
    
  }

}


void InCounter(){
  PCounter = PCounter +1 ;  
}


//*****************************************    SETUP Iniziale degli ingressi e uscite 
void SetupIn (byte Id) {          // Funzione Setup In
      if (iIn[Id].IdPinI != 0xFF) {          
          pinMode(iIn[Id].IdPinI, INPUT);
          digitalWrite(iIn[Id].IdPinI, HIGH);  //questa attivazione serve per attivare la resistenza di pull-up
          digitalRead(iIn[Id].IdPinI) ; 
        }
}

void RWIO(byte Id){
  if (iIn[Id].ActOption){
      bitWrite((iIn[Id].fl),0, !digitalRead(iIn[Id].IdPinI));
  }else{
      bitWrite((iIn[Id].fl),0, digitalRead(iIn[Id].IdPinI));
  }
  if (bitRead(iIn[Id].fl, 0) == true) {     //Controllo se il pulsante si è attivato
    if (bitRead(iIn[Id].fl, 2) == true) {
        if (millis() > iIn[Id].TAct) {                       //controllo se il tempo di Anti rimbalzo è passato
            bitWrite(iIn[Id].fl, 2, false);                   //se è passato allora attivo l'uscita
            bitWrite(iIn[Id].fl, 3, true);
            
        }
    }
  }else{
    iIn[Id].TAct = millis() + Tar;                         //se è cambiato da prima allora aggiorno il tempo di attivazione
    bitWrite(iIn[Id].fl, 2, true);
  }

}
