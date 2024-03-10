#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

LiquidCrystal_I2C lcd(0x27, 20, 4);

TinyGPS gps;
//SoftwareSerial ss(4, 3);

void setup()
{
  Serial.begin(9600);
  lcd.begin();
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    lcd.setCursor(0,0);
    lcd.print("LAT=");
    lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    lcd.print(" LON=");
    lcd.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    /*
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    */
  }
  
  gps.stats(&chars, &sentences, &failed);
  lcd.setCursor(0,2);
  lcd.print(" CHARS=");
  lcd.print(chars);
  lcd.print(" SENTENCES=");
  lcd.print(sentences);
  lcd.print(" CSUM ERR=");
                   lcd.print(failed);
  
  if (chars == 0) {
    lcd.setCursor(0,3);
    lcd.println("** No characters received from GPS: check wiring **");
  }
}
