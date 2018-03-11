
#include <UnoWiFiDevEd.h>


//static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

#include <TinyGPS++.h>
//#include <SoftwareSerial.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);

#define NrSample 30

double LatA=0;
double LngA=0;

double LatB=0;
double LngB=0;

int C1 = 0 ;
int C2 = 0 ;
int iMenu= 0 ;
int BtnA = 0;
int BtnB = 0;




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
unsigned long LtCal;
double Lt_min;
double Lt_mt; // indicazione dei lt per metro percorso
unsigned long LtT1=0, LtT2=0, LtPC=0; //Usate per il calcolo di portata Lt/min


// Pulsanti
int LBZ, LBC,BBZ, BBC;
unsigned long TBZ, TBC;


void setup() {
  TrCal= 0.1 / 1000 ;    // ml/pulse  / 1000  ottengo i litri per impulso ,settaggio dello strumento hendress+hauser
  //ss.begin(GPSBaud);
  Serial.begin(9600);

  pinMode(Btn_Z, INPUT);
  pinMode(Btn_C, INPUT);
  
  lcd.begin(20,4);        // 20 columns by 4 rows on display

  lcd.setBacklight(HIGH); // Turn on backlight, LOW for off

  //lcd.setCursor ( 0, 0 );            // go to the top left corner
  //lcd.print("GPS"); // write this string on the top row

  //Interrupt del contatore
  digitalWrite(INTERRUPT_INPUT, HIGH);
  attachInterrupt(INTERRUPT_INPUT - 2, interrupt_handler, RISING);
  C1=0;
}

void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  BtnA= analogRead(0) ; 
 
//************************************************* Lettura GPS  ********************************  
  //while (ss.available() > 0){
    if (Serial.available() > 0){
      if (gps.encode(Serial.read())){
        lcd.setCursor (0,0);
        if (gps.location.isValid()){
          lcd.print(gps.location.lat(), 4);
          lcd.print(F(","));
          lcd.print(gps.location.lng(), 4);
          lcd.print(F(","));
          
          LatA+= gps.location.lat();
          LngA+= gps.location.lng();
          C1 = C1 + 1;
          lcd.print(C1,0);
          if (C1 == NrSample){
            LatB=LatA/NrSample;
            LngB=LngA/NrSample;
            C1=0;
            LatA=0;
            LngA=0;    
          }
        }else{
          lcd.print(F("INVALID"));
        }
      //******************************** DATA ******************************************
        if (iMenu==0){
          lcd.setCursor (0,1);
          //lcd.print(F("Date:"));
          if (gps.date.isValid()){
            if (gps.date.day() < 10) lcd.print(F("0"));
            lcd.print(gps.date.day());
            lcd.print(F("/"));
            if (gps.date.month() < 10) lcd.print(F("0"));
            lcd.print(gps.date.month());
            lcd.print(F("/"));
            lcd.print(gps.date.year());
            lcd.print(F("  "));
          }else{
            lcd.print(F("INVALID"));
          }
        //********************************   TIME  *****************************************
          //lcd.setCursor (0,2);
          //lcd.print(F("Time:"));
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
      }
  }
      
  if (C2 != C1){
    C2 = C1;
    T2=millis();
    PULSD=pulse_counter - PULS1;
    PULS1=pulse_counter;
    LAT2=LAT1;
    LNG2=LNG1;
    LAT1=0;
    LNG1=0;
    LAT1=LatA;
    LNG1=LngB;
    TD=(T2-T1)/1000;
    T1=millis();
    
    //DIST=Distanza(LAT1,LNG1,LAT2,LNG2);
    //if (DIST > 500){
    //  DIST=502;
    //}
    //SPEED=(DIST/TD); // velocità in mt/sec
    //if (SPEED > 500) {
    //  SPEED=501;
    //}
    
    // misura della portata in litri/min
    //qui ho gli impulsi conteggiati in TD secondi, quindi ongni impulso vale TrCal ml
    //Volume_in_TD_secondi = TrCal * PULSD  ottengo i litri erogati nel tempo TD
    //Volume al minuto = Volume_in_TD_secondi / TD (ottengo il volume al secondo) * 60 (ottengo il volume al minuto)

    //Lt_min= TrCal * PULSD * 60 / TD;
    //P_S=PULSD/TD;

    //Lt_mt= TrCal * PULSD / DIST;
  }

//**********************************************************************************************************
//**********************************************************************************************************
//************************** Calcolo Portata  **************************************************************
//Eseguo il calcolo ogni variazione di almeno 1Lt di prodotto o meglio di LtCal, quindi ogni qualvolta il 
//contatore ha una variazione di almeno LtCal fa il calcolo della portata 
//Quando viene calcolata la nuova portata viene anche inserita nel display, se siamo nel giusto menu

  if ((pulse_counter - LtPC)> LtCal){
    LtT2=millis();
    TD=(LtT2-LtT1)/1000;
    PULSD=pulse_counter - LtPC;
    LtPC=pulse_counter;
    LtT1=millis(); 
    if (TD==0){
      Lt_min=0;   
    }else{
      Lt_min= TrCal * PULSD * 60 / TD;
    }
  }
  


  
//**********************************************************************************************************
//**********************************************************************************************************
//************************** iMenu  ********** Gestione delle funzioni display e tast **********************
  
  //lcd.setCursor (0,1);
  //lcd.print(BtnA);
  //lcd.setCursor (0,2);
  
  if ((BtnA>=0) && (BtnA<100) && (BtnB==0) ) { //Pressione pulsante Giallo
    BtnB=1;
    //lcd.print(F("Prova Giallo"));
    iMenu +=1 ;
    iMenu = iMenu & 3;
    lcd.setCursor (0,0);
    lcd.print(F("                    "));
    lcd.setCursor (0,1);
    lcd.print(F("                    "));
    lcd.setCursor (0,2);
    lcd.print(F("                    "));
    lcd.setCursor (0,3);
    lcd.print(F("                    "));
    
  }

  if ((BtnA>200) && (BtnA<300) && (BtnB==0) ) { //Pressione pulsante Rosso
    BtnB=1;
    //lcd.print(F("Prova Rosso "));

    if (iMenu==0){
      pulse_counter=0;
    }
    lcd.setCursor (0,3);
    lcd.print(F("                    "));

  }

  if (BtnA > 300) {
    BtnB=0;
  }
  
  if (iMenu==0){ // Menu 0
    lcd.setCursor (14,2);
    //lcd.print (F(" Litri/Minuto:"));
    lcd.print (Lt_min,3);    
    
    lcd.setCursor (14,3);
    //lcd.print (F("Totalizzatore:"));
    lcd.print (pulse_counter);    

  
  }
 pulse_counter+=1;
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
