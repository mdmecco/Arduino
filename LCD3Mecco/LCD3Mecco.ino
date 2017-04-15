
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

#define NrSample 30

double LatA[NrSample] = {};
double LngA[NrSample] = {};
int Cl =0 ;
int C2 =0 ;


#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
                          // Address can be changed by soldering A0, A1, or A2
                          // Default is 0x27

// map the pin configuration of LCD backpack for the LiquidCristal class
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define INTERRUPT_INPUT 2
#define Btn_Z 6 // reset il total counter
#define Btn_C 7 // cambia visualizzazione
int attesaDebounce = 50;

#define MSecCampionamento 5000 //Definisce ogni quanto tempo fare il refresh delle misure


LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);

unsigned long pulse_counter = 0;
int n = 1; // a global variable to track number of display refreshes
unsigned long    T1=0, T2=0;
double TD;
double  SPEED=0;
double LAT1, LAT2, LNG1, LNG2;
double DIST;
unsigned long PULS1,PULS2,PULSD;
double P_S;
double Total;
unsigned long PULST;
double TrCal;
double Lt_min;
double Lt_mt; // indicazione dei lt per metro percorso

// Pulsanti
int LBZ, LBC,BBZ, BBC;
unsigned long TBZ, TBC;


void setup() {
  TrCal= 0.1 / 1000 ;    // ml/pulse  / 1000  ottengo i litri per impulso ,settaggio dello strumento hendress+hauser
  ss.begin(GPSBaud);

  pinMode(Btn_Z, INPUT);
  pinMode(Btn_C, INPUT);
  
  lcd.begin(20,4);        // 20 columns by 4 rows on display

  lcd.setBacklight(HIGH); // Turn on backlight, LOW for off

  lcd.setCursor ( 0, 0 );            // go to the top left corner
  lcd.print("GPS"); // write this string on the top row

  //Interrupt del contatore
  digitalWrite(INTERRUPT_INPUT, HIGH);
  attachInterrupt(INTERRUPT_INPUT - 2, interrupt_handler, RISING);
  
}
//*************************************************** LOOP Main *******************************
void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  LBZ=digitalRead(Btn_Z);
  if (LBZ != BBZ and millis() > TBZ){
      TBZ=millis()+attesaDebounce;
      BBZ=LBZ;
    }
  
  
//************************************************* Lettura GPS  ********************************  
  //while (ss.available() > 0){
    if (ss.available() > 0){
      if (gps.encode(ss.read())){
        lcd.setCursor (4,0);
        if (gps.location.isValid()){
          lcd.print(gps.location.lat(), 4);
          lcd.print(F(","));
          lcd.print(gps.location.lng(), 4);
          LatA[Cl]=gps.location.lat();
          LngA[Cl]=gps.location.lng();
          Cl +=1;
          Cl= Cl % NrSample;
          lcd.setCursor (18,2);
          if (Cl >9) {
            lcd.print(Cl);
          }else{
            lcd.print(" ");
            lcd.print(Cl);
          }
        }else{
          lcd.print(F("INVALID"));
        }
        displayDT();
      }
  }
      

//  if (millis() > MSecCampionamento && gps.charsProcessed() < 10)
//  {
//    lcd.setCursor ( 0, 3 );
//    lcd.println(F("No GPS detected: check wiring."));
//    while(true);
//  }

 // if ((millis()-T1) > MSecCampionamento){
  //if ((NrSample-1) == Cl){
  if (C2 != Cl){
    C2 = Cl;
    T2=millis();
    PULSD=pulse_counter - PULS1;
    PULS1=pulse_counter;
    LAT2=LAT1;
    LNG2=LNG1;
    LAT1=0;
    LNG1=0;
    for (int i=0; i <= (NrSample-1); i++){
      LAT1=LAT1 + LatA[i];
      LNG1=LNG1 + LngA[i];
    }
    LAT1=LAT1/NrSample;
    LNG1=LNG1/NrSample;
    TD=(T2-T1)/1000;
    T1=millis();
    
    DIST=Distanza(LAT1,LNG1,LAT2,LNG2);
    if (DIST > 500){
      DIST=502;
    }
    SPEED=(DIST/TD); // velocità in mt/sec
    if (SPEED > 500) {
      SPEED=501;
    }
    
    // misura della portata in litri/min
    //qui ho gli impulsi conteggiati in TD secondi, quindi ongni impulso vale TrCal ml
    //Volume_in_TD_secondi = TrCal * PULSD  ottengo i litri erogati nel tempo TD
    //Volume al minuto = Volume_in_TD_secondi / TD (ottengo il volume al secondo) * 60 (ottengo il volume al minuto)

    Lt_min= TrCal * PULSD * 60 / TD;
    P_S=PULSD/TD;

    Lt_mt= TrCal * PULSD / DIST;
    
  //Cl +=1 ;
  //Cl= Cl % 100;

    lcd.setCursor (0,3);
    lcd.print(SPEED,1);
    lcd.print("    ");
    lcd.print(DIST,2);
    lcd.print("    ");
    //lcd.setCursor (18,3);
  // lcd.print(P_S,1);
  // lcd.print(Cl);
   
  }
}



void displayDT(){
//******************************** DATA ******************************************
  lcd.setCursor (0,1);
  lcd.print(F("Date:"));
  if (gps.date.isValid()){
    if (gps.date.day() < 10) lcd.print(F("0"));
    lcd.print(gps.date.day());
    lcd.print(F("/"));
    if (gps.date.month() < 10) lcd.print(F("0"));
    lcd.print(gps.date.month());
    lcd.print(F("/"));
    lcd.print(gps.date.year());
  }else{
    lcd.print(F("INVALID"));
  }
//********************************   TIME  *****************************************
  lcd.setCursor (0,2);
  lcd.print(F("Time:"));
  if (gps.time.isValid()){
    if (gps.time.hour() < 10) lcd.print(F("0"));
    lcd.print(gps.time.hour());
    lcd.print(F(":"));
    if (gps.time.minute() < 10) lcd.print(F("0"));
    lcd.print(gps.time.minute());
    lcd.print(F(":"));
    if (gps.time.second() < 10) lcd.print(F("0"));
    lcd.print(gps.time.second());
  }else{
    lcd.print(F("INVALID"));
  }
}


/* static */
double Distanza(double lat1, double long1, double lat2, double long2)
{
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = radians(long1-long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
}

void interrupt_handler()
{
  pulse_counter = pulse_counter + 1;
}
