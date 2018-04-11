
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

//double LatA=0;
//double LngA=0;

//double LatB=0;
//double LngB=0;

int C1 = 0 ;
int C2 = 0 ;
int iMenu= 0 ;
int BtnA = 0;
int BtnB = 0;
unsigned long BtnDB=0;




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
#define TrCal= 0.1 / 1000 ;    // ml/pulse  / 1000  ottengo i litri per impulso ,settaggio dello strumento hendress+hauser

LiquidCrystal_I2C lcd(I2C_ADDR, En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, POSITIVE);

unsigned long pulse_counter = 0;
//int n = 1; // a global variable to track number of display refreshes
unsigned long    T1=0, T2=0, T3=0, T4=0;
//double TD;
unsigned long Speed=0; // Velocità in metri al minuto
//unsigned long PULS1,PULS2,PULSD;
//double P_S;
//double Total;
//unsigned long PULST;
//double TrCal;
//unsigned long LtCal;
//double Lt_min;
//double Lt_mt; // indicazione dei lt per metro percorso
//unsigned long LtT1=0, LtT2=0, LtPC=0; //Usate per il calcolo di portata Lt/min
String CmdWifi="";
String BCK="";

int ind1, ind2, ind3, ind4;


// Pulsanti
int LBZ, LBC,BBZ, BBC;
unsigned long TBZ, TBC;
unsigned int WMAS=0;
unsigned long WTIME=0;


void setup() {
  
  //ss.begin(GPSBaud);
  Serial.begin(9600);
  Wifi.begin();
  Wifi.println("Atomizer Server is up");


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
//************************************************* Lettura GPS  ********************************  
  if (Serial.available() > 0){
    gps.encode(Serial.read());
  }

  //*********************************************Lettura Wifi ******************************
  if (Wifi.available()){
    WMAS=1;
    WTIME=millis()+1000;
    CmdWifi= CmdWifi + Wifi.readString();
  }
  
  if ((WMAS==1) && (millis() > WTIME)){
      ind1 = CmdWifi.indexOf('/');  //finds location of first ,
      ind2 = CmdWifi.indexOf('/', ind1+1 );   //finds location of second ,
      ind3 = CmdWifi.indexOf('/', ind2+1 );   //finds location of second ,
      ind1 = CmdWifi.indexOf('/', ind3+1 );   //finds location of second ,
      BCK= CmdWifi.substring(ind3+1, ind1);   //captures first data String
      BCK.toUpperCase();

      Wifi.println("HTTP/1.1 200 OK");
      Wifi.println("Content-Type: text/html");
      Wifi.println("Connection: close");
      //Wifi.println("Refresh: 20");  // refresh the page automatically every  sec
      Wifi.println();
      Wifi.println("<html>");
      Wifi.println("<head> <title>UNO WIFI ->" + BCK + "</title> </head>");
      Wifi.print("<body>");
      Wifi.println("<br/>");
      if (BCK=="GPS"){
        //Wifi.print("Latitude in degrees (double):");
        //Wifi.println( gps.location.lat(), 6); // Latitude in degrees (double)
        //Wifi.println("<br/>");
        //Wifi.print("Longitude in degrees (double):");
        //Wifi.println( gps.location.lng(), 6); // Longitude in degrees (double)
        //Wifi.println("<br/>");
        //Wifi.print("");
        //Wifi.print(gps.location.rawLat().negative ? "-" : "+");
        //Wifi.println("<br/>");
        //Wifi.print("Raw latitude in whole degrees:");
        //Wifi.println( gps.location.rawLat().deg); // Raw latitude in whole degrees
        //Wifi.println("<br/>");
        //Wifi.print("and billionths (u16/u32):");
        //Wifi.println( gps.location.rawLat().billionths);// ... and billionths (u16/u32)
        //Wifi.println("<br/>");
        //Wifi.print("");
        //Wifi.print(gps.location.rawLng().negative ? "-" : "+");
        //Wifi.println("<br/>");
        //Wifi.print("Raw longitude in whole degrees:");
        //Wifi.println(gps.location.rawLng().deg); // Raw longitude in whole degrees
        //Wifi.println("<br/>");
        //Wifi.print("billionths (u16/u32)");
        //Wifi.println( gps.location.rawLng().billionths);// ... and billionths (u16/u32)
        //Wifi.println("<br/>");
        //Wifi.print("Raw date in DDMMYY format (u32):");
        //Wifi.println(gps.date.value()); // Raw date in DDMMYY format (u32)
        //Wifi.println("<br/>");
        //Wifi.print("Year (2000+) (u16)");
        //Wifi.println(gps.date.year()); // Year (2000+) (u16)
        //Wifi.println("<br/>");
        //Wifi.print("Month (1-12) (u8)");
        //Wifi.println(gps.date.month()); // Month (1-12) (u8)
        //Wifi.println("<br/>");
        //Wifi.print("Day (1-31) (u8)");
        //Wifi.println(gps.date.day()); // Day (1-31) (u8)
        Wifi.println("<br/>");
        Wifi.print("Raw time in HHMMSSCC format (u32)");
        Wifi.println(gps.time.value()); // Raw time in HHMMSSCC format (u32)
        //Wifi.print("");
        //Wifi.println("<br/>");
        //Wifi.print("Hour (0-23) (u8)");
        //Wifi.println(gps.time.hour()); // Hour (0-23) (u8)
        //Wifi.println("<br/>");
        //Wifi.print("Minute (0-59) (u8)");
        //Wifi.println(gps.time.minute()); // Minute (0-59) (u8)
        //Wifi.println("<br/>");
        //Wifi.print("Second (0-59) (u8)");
        //Wifi.println(gps.time.second()); // Second (0-59) (u8)
        //Wifi.println("<br/>");
        //Wifi.print("100ths of a second (0-99) (u8)");
        //Wifi.println(gps.time.centisecond()); // 100ths of a second (0-99) (u8)
        //Wifi.println("<br/>");
        //Wifi.print("Raw speed in 100ths of a knot (i32)");
        //Wifi.println(gps.speed.value()); // Raw speed in 100ths of a knot (i32)
        //Wifi.println("<br/>");
        //Wifi.print("Speed in knots (double)");
        //Wifi.println(gps.speed.knots()); // Speed in knots (double)
        //Wifi.println("<br/>");
        //Wifi.print("Speed in miles per hour (double)");
        //Wifi.println(gps.speed.mph()); // Speed in miles per hour (double)
        //Wifi.println("<br/>");
        //Wifi.print("Speed in meters per second (double)");
        //Wifi.println(gps.speed.mps()); // Speed in meters per second (double)
        //Wifi.println("<br/>");
        //Wifi.print("Speed in kilometers per hour (double)");
        //Wifi.println(gps.speed.kmph()); // Speed in kilometers per hour (double)
        //Wifi.println("<br/>");
        //Wifi.print("Raw course in 100ths of a degree (i32)");
        //Wifi.println(gps.course.value()); // Raw course in 100ths of a degree (i32)
        //Wifi.println("<br/>");
        //Wifi.print("Course in degrees (double)");
        //Wifi.println(gps.course.deg()); // Course in degrees (double)
        Wifi.println("<br/>");
        Wifi.print("Raw altitude in centimeters (i32)");
        Wifi.println(gps.altitude.value()); // Raw altitude in centimeters (i32)
        Wifi.println("<br/>");
        //Wifi.print("Altitude in meters (double)");
        //Wifi.println(gps.altitude.meters()); // Altitude in meters (double)
        //Wifi.println("<br/>");
        //Wifi.print("Altitude in miles (double)");
        //Wifi.println(gps.altitude.miles()); // Altitude in miles (double)
        //Wifi.println("<br/>");
        //Wifi.print("Altitude in kilometers (double)");
        //Wifi.println(gps.altitude.kilometers()); // Altitude in kilometers (double)
        //Wifi.println("<br/>");
        //Wifi.print("Altitude in feet (double)");
        //Wifi.println(gps.altitude.feet()); // Altitude in feet (double)
        //Wifi.println("<br/>");
        Wifi.print("Number of satellites in use (u32)");
        Wifi.println(gps.satellites.value()); // Number of satellites in use (u32)
        Wifi.println("<br/>");
        Wifi.print("Horizontal Dim. of Precision (100ths-i32)");
        Wifi.println(gps.hdop.value()); // Horizontal Dim. of Precision (100ths-i32)
      }
      Wifi.println("</body>");
      Wifi.println("</html>");
      Wifi.print(DELIMITER); // very important to end the communication !!!
      CmdWifi="";
      WMAS=0;
   }    
 

  

  //******************************************* Gestione DISPLAY *******************************
  if (T3<millis()){
    T3=millis()+1000;
    lcd.setCursor (0,2);
    lcd.print(BCK);
    //lcd.print("--");
    //lcd.print(BCI);
    //BCI=0;
    
    pulse_counter+=1; // *******************************simulazione contalitri
    if (gps.location.isValid()){
      //lcd.setCursor (0,2);
      //lcd.print(gps.satellites.value());
      lcd.setCursor (0,3);
      lcd.print(gps.location.lat(), 4);
      lcd.print(F(","));
      lcd.print(gps.location.lng(), 4);
      //LatA+= gps.location.lat();
      //LngA+= gps.location.lng();
      lcd.print(F(","));
      Speed=gps.speed.mps()*60;
      lcd.print(Speed);
     }
    //******************************** DATA ******************************************
    if (gps.date.isValid()){
      lcd.setCursor (0,0);
      if (gps.date.day() < 10) lcd.print(F("0"));
      lcd.print(gps.date.day());
      lcd.print(F("/"));
      if (gps.date.month() < 10) lcd.print(F("0"));
      lcd.print(gps.date.month());
      lcd.print(F("/"));
      lcd.print(gps.date.year());
      //lcd.print(F("  "));
    }
    //********************************   TIME  *****************************************
    if (gps.time.isValid()){
      lcd.setCursor (12,0);
      if (gps.time.hour() < 10) lcd.print(F("0"));
      lcd.print(gps.time.hour());
      lcd.print(F(":"));
      if (gps.time.minute() < 10) lcd.print(F("0"));
      lcd.print(gps.time.minute());
      lcd.print(F(":"));
      if (gps.time.second() < 10) lcd.print(F("0"));
      lcd.print(gps.time.second());
    }
  //********************************* Informazioni da display ****************************
  lcd.setCursor (0,1);
  lcd.print (F("Tot:"));
  lcd.print (pulse_counter);

  //lcd.setCursor (0,2);
  //lcd.print (F("Analog::"));
  //lcd.print (BtnA);

  }
      
  //if (C2 != C1){
    //C2 = C1;
    //T2=millis();
    //PULSD=pulse_counter - PULS1;
    //PULS1=pulse_counter;
    //LAT2=LAT1;
    //LNG2=LNG1;
    //LAT1=0;
    //LNG1=0;
    //LAT1=LatA;
    //LNG1=LngB;
    //TD=(T2-T1)/1000;
    //T1=millis();
    
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
  //}

//**********************************************************************************************************
//**********************************************************************************************************
//************************** Calcolo Portata  **************************************************************
//Eseguo il calcolo ogni variazione di almeno 1Lt di prodotto o meglio di LtCal, quindi ogni qualvolta il 
//contatore ha una variazione di almeno LtCal fa il calcolo della portata 
//Quando viene calcolata la nuova portata viene anche inserita nel display, se siamo nel giusto menu

  //if ((pulse_counter - LtPC)> LtCal){
  //  LtT2=millis();
  //  TD=(LtT2-LtT1)/1000;
  //  PULSD=pulse_counter - LtPC;
  //  LtPC=pulse_counter;
  //  LtT1=millis(); 
  //  if (TD==0){
  //    Lt_min=0;   
  //  }else{
  //    Lt_min= TrCal * PULSD * 60 / TD;
  //  }
 // }
  


  
//**********************************************************************************************************
//**********************************************************************************************************
//************************** iMenu  ********** Gestione delle funzioni display e tast **********************
  BtnA= analogRead(0) ; 
  if ((BtnA>=0) && (BtnA<100) && (BtnB==0) ) { //Pressione pulsante Giallo
    BtnB=1;
    if (millis() > BtnDB){
      iMenu +=1 ;
      iMenu = iMenu & 3;
    }
    BtnDB=millis()+attesaDebounce;
  }

  if ((BtnA>200) && (BtnA<300) && (BtnB==0) ) { //Pressione pulsante Rosso
    BtnB=1;
    if (millis() > BtnDB){
      if (iMenu==0){
        pulse_counter=0;
      }
    }
    BtnDB=millis()+attesaDebounce;
  }
  if (BtnA > 300) {
    BtnB=0;
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
