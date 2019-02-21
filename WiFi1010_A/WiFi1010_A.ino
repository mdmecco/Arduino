
unsigned long GetETH =0;
#define ETHT 1000


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  Serial.println("************START**************");
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  digitalWrite(LED_BUILTIN, LOW);
  InitDigitalIn();
}

void loop() {
   if (WifiConnection()){
      digitalWrite(LED_BUILTIN, HIGH);
      MainClient();
      ProcDigitaIn();
      if (millis() > GetETH){
        if (FunzioneOrario(1,0) ==0) {
          GetETH=millis()+ ETHT;
        }
      }
   }
}
