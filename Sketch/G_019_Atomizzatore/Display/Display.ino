#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip

// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

const byte ledPin = 13;
const byte interruptPin = 2;
volatile unsigned long PCounter = 0;

unsigned long TTime=0;




void setup()
{
  Serial.begin(9600);
  Serial.println("Go Interrupt...");
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), InCounter, CHANGE);
  
  Serial.println("Go LCD...");
  lcd.begin(LCD_COLS, LCD_ROWS);
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("It Works!"); 
  lcd.setCursor(0, 3);
  lcd.print("Ciao Vika"); 

  Serial.println("Go main...");
}

void loop() {

  if (millis() > TTime) {
    TTime=millis()+500; 
    lcd.setCursor(0, 2);
    lcd.print(PCounter);  
  }

}


void InCounter(){
  PCounter = PCounter +1 ;  
}
